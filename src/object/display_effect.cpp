//  SuperTux
//  Copyright (C) 2006 Matthias Braun <matze@braunis.de>
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

#include "object/display_effect.hpp"

#include "supertux/globals.hpp"
#include "video/drawing_context.hpp"
#include "video/video_system.hpp"
#include "video/viewport.hpp"

// In menu units, so the bars are the same on every screen and at every zoom
static const float BORDER_SIZE = 90;

DisplayEffect::DisplayEffect(const std::string& name) :
  GameObject(name),
  ExposedObject<DisplayEffect, scripting::DisplayEffect>(this),
  screen_fade(FadeType::NO_FADE),
  screen_fadetime(0),
  screen_fading(0),
  border_fade(FadeType::NO_FADE),
  border_fadetime(0),
  border_fading(),
  border_size(0),
  black(false),
  borders(false)
{
}

DisplayEffect::~DisplayEffect()
{
}

void
DisplayEffect::update(float dt_sec)
{
  switch (screen_fade) {
    case FadeType::NO_FADE:
      break;
    case FadeType::FADE_IN:
      screen_fading -= dt_sec;
      if (screen_fading < 0) {
        screen_fade = FadeType::NO_FADE;
      }
      break;
    case FadeType::FADE_OUT:
      screen_fading -= dt_sec;
      if (screen_fading < 0) {
        screen_fade = FadeType::NO_FADE;
        black = true;
      }
      break;
    default:
      assert(false);
  }

  switch (border_fade) {
    case FadeType::NO_FADE:
      break;
    case FadeType::FADE_IN:
      border_fading -= dt_sec;
      if (border_fading < 0) {
        border_fade = FadeType::NO_FADE;
      }
      border_size = (border_fadetime - border_fading)
        / border_fadetime * BORDER_SIZE;
      break;
    case FadeType::FADE_OUT:
      border_fading -= dt_sec;
      if (border_fading < 0) {
        borders = false;
        border_fade = FadeType::NO_FADE;
      }
      border_size = border_fading / border_fadetime * BORDER_SIZE;
      break;
    default:
      assert(false);
  }
}

void
DisplayEffect::draw(DrawingContext& context)
{
  context.push_transform();
  context.use_ui_space();
  // Past every edge, since a rectangle snaps to whole units and could stop a row of pixels short
  const float left = -1.0f;
  const float top = -1.0f;
  const float right = static_cast<float>(UI_WIDTH) + 1.0f;
  const float bottom = static_cast<float>(UI_HEIGHT) + 1.0f;

  if (black || screen_fade != FadeType::NO_FADE) {
    float alpha;
    if (black) {
      alpha = 1.0f;
    } else {
      switch (screen_fade) {
        case FadeType::FADE_IN:
          alpha = screen_fading / screen_fadetime;
          break;
        case FadeType::FADE_OUT:
          alpha = (screen_fadetime - screen_fading) / screen_fadetime;
          break;
        default:
          alpha = 0.0f; // NOLINT
          assert(false);
      }

      // Same as in fadetoblack.cpp
      alpha = Color::remove_gamma(alpha);
    }
    context.color().draw_filled_rect(Rectf(left, top, right, bottom),
                                     Color(0, 0, 0, alpha), LAYER_GUI - 10);
  }

  if (borders) {
    context.color().draw_filled_rect(Rectf(left, top, right, border_size),
                                       Color(0, 0, 0, 1.0f), LAYER_GUI-10);
    context.color().draw_filled_rect(Rectf(left, static_cast<float>(UI_HEIGHT) - border_size, right, bottom),
                                       Color(0, 0, 0, 1.0f), LAYER_GUI-10);
  }

  context.pop_transform();
}

void
DisplayEffect::fade_out(float fadetime)
{
  black = false;
  screen_fadetime = fadetime;
  screen_fading = fadetime;
  screen_fade = FadeType::FADE_OUT;
}

void
DisplayEffect::fade_in(float fadetime)
{
  black = false;
  screen_fadetime = fadetime;
  screen_fading = fadetime;
  screen_fade = FadeType::FADE_IN;
}

void
DisplayEffect::set_black(bool enabled)
{
  black = enabled;
}

bool
DisplayEffect::is_black() const
{
  return black;
}

float
DisplayEffect::get_border_size() const
{
  return borders ? BORDER_SIZE : 0.0f;
}

void
DisplayEffect::sixteen_to_nine(float fadetime)
{
  if (fadetime == 0) {
    borders = true;
    border_size = BORDER_SIZE;
  } else {
    borders = true;
    border_size = 0;
    border_fade = FadeType::FADE_IN;
    border_fadetime = fadetime;
    border_fading = border_fadetime;
  }
}

void
DisplayEffect::four_to_three(float fadetime)
{
  if (fadetime == 0) {
    borders = false;
  } else {
    border_size = BORDER_SIZE;
    border_fade = FadeType::FADE_OUT;
    border_fadetime = fadetime;
    border_fading = border_fadetime;
  }
}

/* EOF */
