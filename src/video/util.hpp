// src/video/util.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2013 Ingo Ruhnke <grumbel@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_VIDEO_UTIL_HPP
#define HEADER_SUPERTUX_VIDEO_UTIL_HPP

#include "SDL_rect.h"

class Size;
class Vector;

void calculate_viewport(const Size& min_size, const Size& max_size,
                        const Size& real_window_size,
                        float pixel_aspect_ratio, float magnification,
                        Vector& out_scale,
                        Size& out_logical_size,
                        SDL_Rect& out_viewport);

float calculate_pixel_aspect_ratio(const Size& source, const Size& target);

#endif // HEADER_SUPERTUX_VIDEO_UTIL_HPP

// EOF
