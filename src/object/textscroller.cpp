//  SuperTux
//  Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//  Copyright (C) 2018 Ingo Ruhnke <grumbel@gmail.com>
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "object/textscroller.hpp"

#include <SDL.h>
#include <algorithm>
#include <cmath>
#include <optional>
#include <stdexcept>
#include <sexp/value.hpp>

#include "control/input_manager.hpp"
#include "supertux/d_scope.hpp"
#include "supertux/globals.hpp"
#include "supertux/fadetoblack.hpp"
#include "supertux/info_box_line.hpp"
#include "supertux/screen_manager.hpp"
#include "supertux/sector.hpp"
#include "util/log.hpp"
#include "util/reader_collection.hpp"
#include "util/reader_document.hpp"
#include "util/reader_mapping.hpp"
#include "video/drawing_context.hpp"
#include "video/video_system.hpp"
#include "video/viewport.hpp"

namespace {

const float LEFT_BORDER = 0;
const float DEFAULT_SPEED = 60;
const float SCROLL_JUMP = 60;
// Up or down held moves the page this many times faster
const float HELD_SPEED = 5;

/** Sets pace to the pixels each frame that come closest to wanted */
void
pick_pace(float& pace, float wanted)
{
  // The closest of a whole number of pixels every frame or one pixel every whole number of frames
  float frame_pixels = 0.0f;
  if (wanted != 0.0f)
  {
    const float speed = std::abs(wanted);
    const float pixels = std::max(1.0f, std::round(speed));
    const float every = 1.0f / std::max(1.0f, std::round(1.0f / speed));
    const float best = (std::abs(std::log(pixels / speed)) <= std::abs(std::log(every / speed))) ? pixels : every;
    frame_pixels = std::copysign(best, wanted);
  }

  // Kept unless clearly off, so a pace near the middle of two doesn't flip between them
  if (pace == 0.0f || frame_pixels == 0.0f || std::abs(wanted / pace - 1.0f) > 0.4f)
  {
    pace = frame_pixels;
  }
}

} // namespace

TextScroller::TextScroller(const ReaderMapping& mapping) :
  controller(&InputManager::current()->get_controller()),
  m_filename(),
  m_finish_script(),
  m_lines(),
  m_scroll(),
  m_last_scroll(),
  m_drawn(),
  m_last_target(),
  m_frame_pixels(),
  m_held_pixels(),
  m_frame_time(),
  m_pixel_scale(),
  m_last_draw(),
  m_frame_times(),
  m_frame_count(),
  m_even_pace(),
  m_held(),
  m_default_speed(DEFAULT_SPEED),
  m_x_offset(),
  m_controllable(true),
  m_finished(),
  m_fading(),
  m_x_anchor(XAnchor::SCROLLER_ANCHOR_CENTER),
  m_text_align(TextAlign::SCROLLER_ALIGN_CENTER)
{
  if (!mapping.get("file", m_filename))
  {
    log_warning << mapping.get_doc().get_filename() << "'file' tag missing" << std::endl;
  }
  else
  {
    parse_file(m_filename);
  }

  mapping.get("finish-script", m_finish_script, "");
  mapping.get("speed", m_default_speed);
  mapping.get("x-offset", m_x_offset);
  mapping.get("controllable", m_controllable, true);

  std::string x_anchor_str;
  if (mapping.get("x-anchor", x_anchor_str))
  {
    if (x_anchor_str == "left")
      m_x_anchor = XAnchor::SCROLLER_ANCHOR_LEFT;
    else if (x_anchor_str == "right")
      m_x_anchor = XAnchor::SCROLLER_ANCHOR_RIGHT;
    else
      m_x_anchor = XAnchor::SCROLLER_ANCHOR_CENTER;
  }

  std::string text_align_str;
  if (mapping.get("text-align", text_align_str))
  {
    if (text_align_str == "left")
      m_text_align = TextAlign::SCROLLER_ALIGN_LEFT;
    else if (text_align_str == "right")
      m_text_align = TextAlign::SCROLLER_ALIGN_RIGHT;
    else
      m_text_align = TextAlign::SCROLLER_ALIGN_CENTER;
  }

}

TextScroller::TextScroller(const ReaderObject& root) :
  controller(&InputManager::current()->get_controller()),
  m_filename(),
  m_finish_script(),
  m_lines(),
  m_scroll(),
  m_last_scroll(),
  m_drawn(),
  m_last_target(),
  m_frame_pixels(),
  m_held_pixels(),
  m_frame_time(),
  m_pixel_scale(),
  m_last_draw(),
  m_frame_times(),
  m_frame_count(),
  m_even_pace(),
  m_held(),
  m_default_speed(DEFAULT_SPEED),
  m_x_offset(),
  m_controllable(true),
  m_finished(),
  m_fading(),
  m_x_anchor(XAnchor::SCROLLER_ANCHOR_CENTER),
  m_text_align(TextAlign::SCROLLER_ALIGN_CENTER)
{
  parse_root(root);
}

void
TextScroller::parse_file(const std::string& filename)
{
  auto doc = ReaderDocument::from_file(filename);
  auto root = doc.get_root();
  parse_root(root);
}

void
TextScroller::parse_root(const ReaderObject& root)
{
  if (root.get_name() != "supertux-text")
  {
    throw std::runtime_error("File isn't a supertux-text file");
  }
  else
  {
    auto mapping = root.get_mapping();

    int version = 1;
    mapping.get("version", version);
    if (version == 1)
    {
      log_info << "[" << mapping.get_doc().get_filename() << "] Text uses old format: version 1" << std::endl;

      std::string text;
      if (!mapping.get("text", text)) {
        throw std::runtime_error("File doesn't contain a text field");
      }

      // Split text string lines into a vector
      m_lines = InfoBoxLine::split(text, static_cast<float>(UI_WIDTH) - 2.0f * LEFT_BORDER);
    }
    else if (version == 2)
    {
      std::optional<ReaderCollection> content_collection;
      if (!mapping.get("content", content_collection)) {
        throw std::runtime_error("File doesn't contain content");
      } else {
        parse_content(*content_collection);
      }
    }
  }
}

void
TextScroller::parse_content(const ReaderCollection& collection)
{
  for (const auto& item : collection.get_objects())
  {
    if (item.get_name() == "image")
    {
      std::string image_file = item.get_sexp().as_array()[1].as_string();
      m_lines.push_back(std::make_unique<InfoBoxLine>('!', image_file));
    }
    else if (item.get_name() == "person")
    {
      bool simple;
      std::string name, info, image_file;

      item.get_mapping().get("simple", simple, false);

      if (simple) {
        if (!item.get_mapping().get("name", name) || !item.get_mapping().get("info", info)) {
          throw std::runtime_error("Simple entry requires both name and info specified");
        }

        if (item.get_mapping().get("image", image_file)) {
          log_warning << "[" << collection.get_doc().get_filename() << "] Simple person entry shouldn't specify images" << std::endl;
        }

        m_lines.push_back(std::make_unique<InfoBoxLine>(' ', name + " (" + info + ")")); // NOLINT
      } else {
        if (item.get_mapping().get("name", name)) {
          m_lines.push_back(std::make_unique<InfoBoxLine>('\t', name));
        }

        if (item.get_mapping().get("image", image_file) && !simple) {
          m_lines.push_back(std::make_unique<InfoBoxLine>('!', image_file));
        }

        if (item.get_mapping().get("info", info)) {
          m_lines.push_back(std::make_unique<InfoBoxLine>(' ', info));
        }
      }
    }
    else if (item.get_name() == "blank")
    {
      // Empty line
      m_lines.push_back(std::make_unique<InfoBoxLine>('\t', ""));
    }
    else if (item.get_name() == "text")
    {
      std::string type, string;

      if (!item.get_mapping().get("type", type)) {
        type = "normal";
      }

      if (!item.get_mapping().get("string", string)) {
        throw std::runtime_error("Text entry requires a string");
      }

      if (type == "normal")
        m_lines.push_back(std::make_unique<InfoBoxLine>('\t', string));
      else if (type == "normal-left")
        m_lines.push_back(std::make_unique<InfoBoxLine>('#', string));
      else if (type == "small")
        m_lines.push_back(std::make_unique<InfoBoxLine>(' ', string));
      else if (type == "heading")
        m_lines.push_back(std::make_unique<InfoBoxLine>('-', string));
      else if (type == "reference")
        m_lines.push_back(std::make_unique<InfoBoxLine>('*', string));
      else {
        log_warning << "[" << item.get_doc().get_filename() << "] Unknown text type '" << type << "'" << std::endl;
        m_lines.push_back(std::make_unique<InfoBoxLine>('\t', string));
      }
    }
    else
    {
      log_warning << "[" << item.get_doc().get_filename() << "] Unknown token '" << item.get_name() << "'" << std::endl;
    }
  }
}

void
TextScroller::draw(DrawingContext& context)
{
  context.push_transform();
  context.use_ui_space();

  const float ctx_w = static_cast<float>(context.get_width());
  const float ctx_h = static_cast<float>(context.get_height());

  // Part way through the last step
  const float fraction = d_gameobject_manager ? get_step_fraction() : g_step_fraction;
  m_pixel_scale = VideoSystem::current()->get_viewport().get_scale().y * context.transform().scale;
  const float target = (m_last_scroll + (m_scroll - m_last_scroll) * fraction) * m_pixel_scale;
  pick_frame_pixels();

  // The same move every frame, so the page never stalls or skips, nudged a pixel if it drifts from the clock
  const float step = std::abs((m_held != 0) ? m_held_pixels : m_frame_pixels);
  const float forward = ((m_default_speed < 0.0f) ? -1.0f : 1.0f) * ((m_held < 0) ? -1.0f : 1.0f);
  // How far behind the clock before the usual step and after it
  const float behind = (target - m_drawn) * forward;
  const float after = behind - step;
  if (!m_even_pace || after > 8.0f)
  {
    // Until the screen's pace is known, or after a jump ahead, it follows the clock but only ever forwards
    if (behind > 0.0f)
    {
      m_drawn = std::round(target);
    }
  }
  else if (target != m_last_target)
  {
    // Behind takes an extra pixel and ahead goes only 2 pixels past the clock, so it never steps backwards
    m_drawn += forward * ((after > 2.0f) ? step + 1.0f : (after < -2.0f) ? std::max(0.0f, behind + 2.0f) : step);
  }
  m_last_target = target;

  // Every line moves on the pixel grid as one piece
  const float scroll = std::round(m_drawn) / m_pixel_scale;
  context.set_translation(Vector(0.0f, scroll));
  context.transform().pixel_grid = true;

  float y = ctx_h;

  { // draw text
    for (const auto& line : m_lines)
    {
      if (y - scroll + line->get_height() >= 0 && ctx_h - (y - scroll) >= 0) {
        line->draw(context, Rectf(LEFT_BORDER, y, ctx_w * (m_x_anchor == XAnchor::SCROLLER_ANCHOR_LEFT ? 0.f :
           m_x_anchor == XAnchor::SCROLLER_ANCHOR_RIGHT ? 2.f : 1.f) + m_x_offset, y), LAYER_GUI,
          (m_text_align == TextAlign::SCROLLER_ALIGN_LEFT ? line->LineAlignment::LEFT :
           m_text_align == TextAlign::SCROLLER_ALIGN_RIGHT ? line->LineAlignment::RIGHT :
           line->LineAlignment::CENTER));
      }

      y += floorf(line->get_height());
    }
  }

  context.pop_transform();

  // close when done
  if (y - m_scroll < 0)
  {
    m_finished = true;
    set_default_speed(0.f);
  }
}

void
TextScroller::update(float dt_sec)
{
  m_last_scroll = m_scroll;

  // At the pace of the whole pixels it moves each frame
  float speed = m_default_speed;
  m_even_pace = (m_default_speed != 0.0f && m_frame_pixels != 0.0f && m_frame_time > 0.0f && m_pixel_scale > 0.0f);
  if (m_even_pace)
  {
    speed = m_frame_pixels / m_frame_time / m_pixel_scale;
  }

  m_held = 0;
  if (controller && m_controllable) {
    // allow changing speed with up and down keys
    if (controller->hold(Control::UP)) {
      m_held = -1;
    } else if (controller->hold(Control::DOWN)) {
      m_held = 1;
    }
    // At whole pixels each frame too, once the screen's pace is known
    if (m_held != 0) {
      const float held_speed = m_even_pace ? m_held_pixels / m_frame_time / m_pixel_scale : m_default_speed * HELD_SPEED;
      speed = static_cast<float>(m_held) * held_speed;
    }

    // allow jumping ahead with certain keys
    if ((controller->pressed(Control::JUMP) ||
         controller->pressed(Control::ACTION) ||
         controller->pressed(Control::MENU_SELECT)) &&
        !(controller->pressed(Control::UP))) { // prevent skipping if jump with up is enabled
      scroll(SCROLL_JUMP);
    }

    // use start or escape keys to exit
    if ((controller->pressed(Control::START) ||
        controller->pressed(Control::ESCAPE)) &&
        !m_fading  && m_finish_script.empty()) {
      m_fading = true;
      ScreenManager::current()->pop_screen(std::make_unique<FadeToBlack>(FadeToBlack::FADEOUT, 0.5f));
      return;
    }
  }

  m_scroll += speed * dt_sec;

  if (m_scroll < 0)
    m_scroll = 0;
  if (!m_finish_script.empty())
  {
    Sector::get().run_script(m_finish_script, "finishscript");
  }
  else
  {
    // close when done
    if (m_finished && !m_fading)
    {
      m_fading = true;
      ScreenManager::current()->pop_screen(std::make_unique<FadeToBlack>(FadeToBlack::FADEOUT, 0.25f));
    }
  }
}

void
TextScroller::pick_frame_pixels()
{
  const std::uint64_t now = SDL_GetPerformanceCounter();
  if (m_last_draw != 0)
  {
    const float dt = static_cast<float>(static_cast<double>(now - m_last_draw) / static_cast<double>(SDL_GetPerformanceFrequency()));
    // A long gap is a pause or a load, not the screen's pace
    if (dt > 0.0f && dt < 0.25f)
    {
      m_frame_times[m_frame_count % m_frame_times.size()] = dt;
      ++m_frame_count;
    }
  }
  m_last_draw = now;

  // The middle of the last few frames, so a slow one while the level warms up doesn't set the pace
  const size_t samples = std::min(m_frame_count, m_frame_times.size());
  if (samples < m_frame_times.size() / 2)
  {
    return;
  }
  std::array<float, 15> sorted = m_frame_times;
  std::nth_element(sorted.begin(), sorted.begin() + samples / 2, sorted.begin() + samples);
  m_frame_time = sorted[samples / 2];

  const float wanted = m_default_speed * m_pixel_scale * m_frame_time;
  pick_pace(m_frame_pixels, wanted);
  pick_pace(m_held_pixels, wanted * HELD_SPEED);
}

void
TextScroller::set_default_speed(float default_speed)
{
  m_default_speed = default_speed;
}

void
TextScroller::scroll(float offset)
{
  m_scroll += offset;
  if (m_scroll < 0.0f)
  {
    m_scroll = 0.0f;
  }
}

/* EOF */
