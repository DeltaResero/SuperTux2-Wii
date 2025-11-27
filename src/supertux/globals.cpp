// src/supertux/globals.cpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2009 Ingo Ruhnke <grumbel@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#include "supertux/globals.hpp"

#include "supertux/gameconfig.hpp"

int SCREEN_WIDTH;
int SCREEN_HEIGHT;

std::unique_ptr<Config> g_config;

float game_time = 0;
float real_time = 0;

float g_game_speed = 1.0f;

// EOF
