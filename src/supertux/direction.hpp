// src/supertux/direction.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_SUPERTUX_DIRECTION_HPP
#define HEADER_SUPERTUX_SUPERTUX_DIRECTION_HPP

#include <iostream>

class ObjectOption;

enum Direction { AUTO, LEFT, RIGHT, UP, DOWN };

std::ostream& operator<<(std::ostream& o, const Direction& dir);

ObjectOption dir_option(Direction *dir);

std::string dir_to_string(const Direction& dir);
std::string dir_to_prefixed(const Direction& dir, const std::string& prefix);
Direction string_to_dir(const std::string& dir_str);

#endif // HEADER_SUPERTUX_SUPERTUX_DIRECTION_HPP

// EOF
