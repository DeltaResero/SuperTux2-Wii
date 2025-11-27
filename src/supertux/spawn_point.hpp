// src/supertux/spawn_point.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_SUPERTUX_SPAWN_POINT_HPP
#define HEADER_SUPERTUX_SUPERTUX_SPAWN_POINT_HPP

#include <string>

#include "math/vector.hpp"

class ReaderMapping;
class Writer;

class SpawnPoint
{
public:
  SpawnPoint();
  SpawnPoint(const SpawnPoint& other);
  SpawnPoint(const ReaderMapping& lisp);

  void save(Writer& writer);

  std::string name;
  Vector pos;
};

#endif // HEADER_SUPERTUX_SUPERTUX_SPAWN_POINT_HPP

// EOF
