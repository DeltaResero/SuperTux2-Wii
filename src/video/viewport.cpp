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

#include "viewport.hpp"

#include <algorithm>

#include "config.h"

#include "math/rect.hpp"
#include "math/size.hpp"
#include "math/vector.hpp"
#include "supertux/gameconfig.hpp"
#include "supertux/globals.hpp"

// How tall a 1920x1080 screen showed a level; the view must stay inside X/Y_OFFSCREEN_DISTANCE or enemies spawn on screen
const float Viewport::s_logical_height = 768.0f;
const float Viewport::s_min_zoom = 0.9f;
const float Viewport::s_max_zoom = 1.1f;
const float Viewport::s_min_aspect = 5.0f / 4.0f;
const float Viewport::s_max_aspect = 16.0f / 9.0f;

// PAL's height, so menu rows fall on whole lines of common screens and every row stretches the same
const float Viewport::s_ui_height = 576.0f;

namespace {

inline Size
apply_pixel_aspect_ratio_pre(const Size& window_size, float pixel_aspect_ratio)
{
  return Size(static_cast<int>(static_cast<float>(window_size.width) * pixel_aspect_ratio),
              window_size.height);
}

inline void
apply_pixel_aspect_ratio_post(const Size& real_window_size, const Size& window_size, const Vector& scale,
                              Rect& out_viewport, Vector& out_scale)
{
  Vector transform(static_cast<float>(real_window_size.width) / static_cast<float>(window_size.width),
                   static_cast<float>(real_window_size.height) / static_cast<float>(window_size.height));

  out_viewport.left = static_cast<int>(static_cast<float>(out_viewport.left) * transform.x);
  out_viewport.top = static_cast<int>(static_cast<float>(out_viewport.top) * transform.y);
  out_viewport.right = static_cast<int>(static_cast<float>(out_viewport.right) * transform.x);
  out_viewport.bottom = static_cast<int>(static_cast<float>(out_viewport.bottom) * transform.y);

  out_scale.x = scale.x * transform.x;
  out_scale.y = scale.y * transform.y;
}

void calculate_viewport(const Size& real_window_size,
                        float pixel_aspect_ratio, float magnification,
                        Vector& out_scale,
                        Rect& out_viewport)
{
  // Transform the real window_size by the aspect ratio, then do
  // calculations on that virtual window_size
  Size window_size = apply_pixel_aspect_ratio_pre(real_window_size, pixel_aspect_ratio);
  const float window_width = static_cast<float>(window_size.width);
  const float window_height = static_cast<float>(window_size.height);

  // The zoom sets how tall the level is drawn and the screen's shape sets how wide, never its size
  const float zoom = (magnification == 0.0f) ? 1.0f : std::clamp(magnification, Viewport::s_min_zoom, Viewport::s_max_zoom);
  // A shape outside that range is drawn at the nearest end and stretched to fill the window
  const float aspect = std::clamp(window_width / window_height, Viewport::s_min_aspect, Viewport::s_max_aspect);
  const float height = Viewport::s_logical_height / zoom;
  const float width = height * aspect;

  out_viewport.left = 0;
  out_viewport.top = 0;
  out_viewport.right = window_size.width;
  out_viewport.bottom = window_size.height;

  // Transform the virtual window_size back into real window coordinates
  apply_pixel_aspect_ratio_post(real_window_size, window_size,
                                Vector(window_width / width, window_height / height),
                                out_viewport, out_scale);
}

float calculate_pixel_aspect_ratio(const Size& source, const Size& target)
{
  float source_aspect = 16.0f / 9.0f; // random guess
  if (source != Size(0, 0))
  {
    source_aspect =
      static_cast<float>(source.width) /
      static_cast<float>(source.height);
  }

  float target_aspect =
    static_cast<float>(target.width) /
    static_cast<float>(target.height);

  return target_aspect / source_aspect;
}

} // namespace

Viewport
Viewport::from_size(const Size& target_size, const Size& desktop_size)
{
  // A fullscreen mode is stretched over the whole display, so its pixels take the display's shape
  float pixel_aspect_ratio = 1.0f;
  if (g_config->aspect_size != Size(0, 0))
  {
    pixel_aspect_ratio = calculate_pixel_aspect_ratio(g_config->use_fullscreen ? target_size : desktop_size,
                                                      g_config->aspect_size);
  }
  else if (g_config->use_fullscreen && desktop_size != Size(0, 0))
  {
    pixel_aspect_ratio = calculate_pixel_aspect_ratio(target_size,
                                                      desktop_size);
  }

  // calculate the viewport
  Rect viewport;
  Vector scale(0.0f, 0.0f);
  calculate_viewport(target_size,
                     pixel_aspect_ratio,
                     g_config->magnification,
                     scale, viewport);

  // Menus, the HUD and text fill the window's height the same way on every screen, whatever the zoom
  const float ui_scale = static_cast<float>(target_size.height) / s_ui_height;

  return Viewport(viewport, scale, ui_scale / scale.y);
}

Viewport::Viewport() :
  m_rect(),
  m_scale(0.0f, 0.0f),
  m_ui_scale(1.0f)
{
}

Viewport::Viewport(const Rect& rect, const Vector& scale, float ui_scale) :
  m_rect(rect),
  m_scale(scale),
  m_ui_scale(ui_scale)
{
}

int
Viewport::get_screen_width() const
{
  return static_cast<int>(static_cast<float>(m_rect.get_width()) / m_scale.x);
}

int
Viewport::get_screen_height() const
{
  return static_cast<int>(static_cast<float>(m_rect.get_height()) / m_scale.y);
}

Size
Viewport::get_screen_size() const
{
  return Size(get_screen_width(), get_screen_height());
}

Vector
Viewport::to_logical(int physical_x, int physical_y) const
{
  return Vector(static_cast<float>(physical_x - m_rect.left) / m_scale.x,
                static_cast<float>(physical_y - m_rect.top) / m_scale.y);
}

int
Viewport::get_ui_width() const
{
  return static_cast<int>(static_cast<float>(get_screen_width()) / m_ui_scale);
}

int
Viewport::get_ui_height() const
{
  return static_cast<int>(static_cast<float>(get_screen_height()) / m_ui_scale);
}

Vector
Viewport::to_ui(int physical_x, int physical_y) const
{
  return to_logical(physical_x, physical_y) / m_ui_scale;
}

/* EOF */
