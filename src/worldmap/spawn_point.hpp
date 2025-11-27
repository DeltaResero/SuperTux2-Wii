// src/worldmap/spawn_point.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux - Worldmap Spawnpoint
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_WORLDMAP_SPAWN_POINT_HPP
#define HEADER_SUPERTUX_WORLDMAP_SPAWN_POINT_HPP

#include <string>

#include "util/reader_fwd.hpp"
#include "math/vector.hpp"
#include "supertux/game_object.hpp"
#include "worldmap/direction.hpp"

namespace worldmap {

class SpawnPoint
{
public:
  SpawnPoint(const ReaderMapping& lisp);

  std::string name;
  Vector pos;
  Direction auto_dir; /**< automatically start walking in this direction */
};

}

#endif // HEADER_SUPERTUX_WORLDMAP_SPAWN_POINT_HPP

// EOF
