// src/worldmap/direction.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux - Worldmap Direction
// Copyright (C) 2006 Christoph Sommer <christoph.sommer@2006.expires.deltadevelopment.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_WORLDMAP_DIRECTION_HPP
#define HEADER_SUPERTUX_WORLDMAP_DIRECTION_HPP

#include <string>

class ObjectOption;

namespace worldmap {

enum Direction { D_NONE, D_WEST, D_EAST, D_NORTH, D_SOUTH };

Direction reverse_dir(Direction direction);
Direction string_to_direction(const std::string& directory);
std::string direction_to_string(Direction direction);

ObjectOption dir_option(Direction *dir);

} // namespace worldmap

#endif // HEADER_SUPERTUX_WORLDMAP_DIRECTION_HPP

// EOF
