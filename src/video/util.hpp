// src/video/util.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
//  SuperTux
//  Copyright (C) 2013 Ingo Ruhnke <grumbel@gmail.com>
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

#ifndef HEADER_SUPERTUX_VIDEO_UTIL_HPP
#define HEADER_SUPERTUX_VIDEO_UTIL_HPP

#include "SDL_rect.h"

class Size;
class Vector;

/** How tall the game draws itself, before any zoom. Everything else follows
    from this: the width comes from the shape of the screen, and how many
    pixels the screen has decides only how sharp the result is. */
static const int LOGICAL_HEIGHT = 600;

/** How far the zoom may go either way. Zooming out is what lets a player see
    further than the level was drawn for, so the limit is the game's, not the
    screen's. */
static const float ZOOM_MIN = 0.5f;
static const float ZOOM_MAX = 2.0f;

/** The shapes the game will draw itself in. Anything wider is drawn at the
    widest and stretched, since the height is fixed and a wider shape is the
    one way a screen could come to show more of a level than another. The
    narrow end is only a guard against a nonsensical number in a config file,
    as a narrower shape shows less rather than more. */
static const float ASPECT_MIN = 5.0f / 4.0f;
static const float ASPECT_MAX = 16.0f / 9.0f;

/** Work out what area of the level to draw, and how to map it onto the
    window.

    @param window_size  the window, in real pixels
    @param aspect_ratio the shape to draw in, or 0 to take the window's own
    @param zoom         1 draws the standard area, more draws less of it
    @param out_scale    what to multiply drawing coordinates by, per axis
    @param out_logical_size  the area of the level that will be drawn
    @param out_viewport the part of the window drawn into, which is all of it */
void calculate_viewport(const Size& window_size,
                        float aspect_ratio, float zoom,
                        Vector& out_scale,
                        Size& out_logical_size,
                        SDL_Rect& out_viewport);

#endif

/* EOF */
