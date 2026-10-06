//  SuperTux
//  Copyright (C) 2016 Ingo Ruhnke <grumbel@gmail.com>
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

#ifndef HEADER_SUPERTUX_VIDEO_VIEWPORT_HPP
#define HEADER_SUPERTUX_VIDEO_VIEWPORT_HPP

#include "math/rect.hpp"
#include "math/vector.hpp"

class Viewport final
{
private:
public:
  static Viewport from_size(const Size& target_size, const Size& desktop_size);

public:
  Viewport();
  Viewport(const Rect& rect, const Vector& scale, float ui_scale = 1.0f);

  /** The size of the viewport in window coordinates */
  Rect get_rect() const { return m_rect; }

  /** The amount by which the content of the viewport is scaled */
  Vector get_scale() const { return m_scale; }

  /** The width of the resulting logical screen */
  int get_screen_width() const;

  /** The height of the resulting logical screen */
  int get_screen_height() const;

  /** The size of the resulting logical screen */
  Size get_screen_size() const;

  /** Converts window coordinates into logical screen coordinates */
  Vector to_logical(int physical_x, int physical_y) const;

  /** How much larger menus, the HUD and text are drawn than the level */
  float get_ui_scale() const { return m_ui_scale; }

  /** The screen in the units menus, the HUD and text are laid out in */
  int get_ui_width() const;
  int get_ui_height() const;

  /** Converts window coordinates into the units menus, the HUD and text are laid out in */
  Vector to_ui(int physical_x, int physical_y) const;

public:
  /** How tall a level is drawn at 100% zoom; the width follows the screen's shape */
  static const float s_logical_height;

  /** The zoom range a player can pick from */
  static const float s_min_zoom;
  static const float s_max_zoom;

  /** The narrowest and widest shapes drawn; a screen outside them is stretched to fit */
  static const float s_min_aspect;
  static const float s_max_aspect;

  /** The range menus, the HUD and text are laid out in */
  static const Size s_ui_min_size;
  static const Size s_ui_max_size;

private:
  Rect m_rect;
  Vector m_scale;
  float m_ui_scale;
};

#endif

/* EOF */
