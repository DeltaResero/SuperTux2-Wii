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

#ifndef HEADER_SUPERTUX_OBJECT_TEXTSCROLLER_HPP
#define HEADER_SUPERTUX_OBJECT_TEXTSCROLLER_HPP

#include <array>
#include <cstdint>
#include <memory>
#include <vector>

#include "supertux/info_box_line.hpp"
#include "supertux/game_object.hpp"
#include "control/controller.hpp"

class DrawingContext;
class InfoBoxLine;
class ReaderCollection;
class ReaderMapping;
class ReaderObject;

class TextScroller : public GameObject
{
public:
  TextScroller(const ReaderMapping& mapping);
  TextScroller(const ReaderObject& root);

  virtual void draw(DrawingContext& context) override;
  virtual void update(float dt_sec) override;
  virtual std::string get_class() const override { return "textscroller"; }

  void set_default_speed(float default_speed);
  void scroll(float offset);

protected:
  const Controller* controller;

private:
  void parse_file(const std::string& filename);
  void parse_root(const ReaderObject& root);
  void parse_content(const ReaderCollection& collection);

  /** Picks the whole pixels each frame that come closest to the speed at the screen's pace */
  void pick_frame_pixels();

private:
  std::string m_filename;
  std::string m_finish_script;
  std::vector<std::unique_ptr<InfoBoxLine> > m_lines;
  float m_scroll;
  float m_last_scroll;

  /** Where the page was drawn and where it should be, in screen pixels */
  float m_drawn;
  float m_last_target;

  /** Pixels the page moves each frame, a fraction for one pixel every few frames */
  float m_frame_pixels;
  float m_frame_time;
  float m_pixel_scale;
  std::uint64_t m_last_draw;
  std::array<float, 15> m_frame_times;
  size_t m_frame_count;
  bool m_even_pace;
  bool m_keys_held;
  float m_default_speed;
  float m_x_offset;
  bool m_controllable;
  bool m_finished;
  bool m_fading;

  enum XAnchor {
    SCROLLER_ANCHOR_LEFT,
    SCROLLER_ANCHOR_CENTER,
    SCROLLER_ANCHOR_RIGHT
  };
  enum TextAlign {
    SCROLLER_ALIGN_LEFT,
    SCROLLER_ALIGN_CENTER,
    SCROLLER_ALIGN_RIGHT
  };

  XAnchor m_x_anchor;
  TextAlign m_text_align;

private:
  TextScroller(const TextScroller&) = delete;
  TextScroller& operator=(const TextScroller&) = delete;
};

#endif

/* EOF */
