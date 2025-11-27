// src/supertux/globals.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2009 Ingo Ruhnke <grumbel@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_SUPERTUX_GLOBALS_HPP
#define HEADER_SUPERTUX_SUPERTUX_GLOBALS_HPP

#include <memory>

class Config;

/** The width of the display (this is a logical value, not the
    physical value, since aspect_ration and projection_area might
    shrink or scale things) */
extern int SCREEN_WIDTH;

/** The width of the display (this is a logical value, not the
    physical value, since aspect_ration and projection_area might
    shrink or scale things) */
extern int SCREEN_HEIGHT;

extern std::unique_ptr<Config> g_config;

extern float game_time;
extern float real_time;

extern float g_game_speed;

#endif // HEADER_SUPERTUX_SUPERTUX_GLOBALS_HPP

// EOF
