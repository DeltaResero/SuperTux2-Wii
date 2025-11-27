// src/worldmap/teleporter.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux - Teleporter Worldmap Tile
// Copyright (C) 2006 Christoph Sommer <christoph.sommer@2006.expires.deltadevelopment.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_WORLDMAP_TELEPORTER_HPP
#define HEADER_SUPERTUX_WORLDMAP_TELEPORTER_HPP

#include <memory>
#include <string>

#include "math/vector.hpp"
#include "supertux/game_object.hpp"
#include "util/reader_fwd.hpp"

class Sprite;

namespace worldmap {

class Teleporter : public GameObject
{
public:
  Teleporter(const ReaderMapping& lisp);

  virtual void draw(DrawingContext& context);
  virtual void update(float elapsed_time);

public:
  /** Position (in tiles, not pixels) */
  Vector pos;

  /** Sprite to render, or 0 for no sprite */
  SpritePtr sprite;

  /** Worldmap filename (relative to data root) to teleport to. Leave empty to use current word */
  std::string worldmap;

  /** Spawnpoint to teleport to. Leave empty to use "main" or last one */
  std::string spawnpoint;

  /** true if this teleporter does not need to be activated, but teleports Tux as soon as it's touched */
  bool automatic;

  /** optional map message to display */
  std::string message;
};

} // namespace worldmap

#endif // HEADER_SUPERTUX_WORLDMAP_TELEPORTER_HPP

// EOF
