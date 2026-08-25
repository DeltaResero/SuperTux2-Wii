// src/video/util.cpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
//  SuperTux
//  Copyright (C) 2014 Ingo Ruhnke <grumbel@gmail.com>
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

#include "video/util.hpp"

#include <algorithm>

#include "math/size.hpp"
#include "math/vector.hpp"

void calculate_viewport(const Size& window_size,
                        float aspect_ratio, float zoom,
                        Vector& out_scale,
                        Size& out_logical_size,
                        SDL_Rect& out_viewport)
{
  if (zoom <= 0.0f) // magic value, meaning no zoom was asked for
  {
    zoom = 1.0f;
  }
  zoom = std::min(std::max(zoom, ZOOM_MIN), ZOOM_MAX);

  /* A screen wider or narrower than the game draws for is filled by
     stretching, so that every screen shows the same amount of level for its
     shape and none of them get a border. */
  if (aspect_ratio <= 0.0f) // magic value, meaning take the window's shape
  {
    aspect_ratio = static_cast<float>(window_size.width) /
                   static_cast<float>(window_size.height);
  }
  aspect_ratio = std::min(std::max(aspect_ratio, ASPECT_MIN), ASPECT_MAX);

  /* The height is what the zoom acts on, and the width follows from the
     shape, so how many pixels the screen has never enters into how much of
     the level is on it. */
  out_logical_size.height = static_cast<int>(LOGICAL_HEIGHT / zoom);
  out_logical_size.width  = static_cast<int>(out_logical_size.height * aspect_ratio);

  /* All of the window is drawn into. There is nowhere for a border to be. */
  out_viewport.x = 0;
  out_viewport.y = 0;
  out_viewport.w = window_size.width;
  out_viewport.h = window_size.height;

  out_scale.x = static_cast<float>(window_size.width) /
                static_cast<float>(out_logical_size.width);
  out_scale.y = static_cast<float>(window_size.height) /
                static_cast<float>(out_logical_size.height);
}

/* EOF */
