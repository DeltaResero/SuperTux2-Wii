// src/object/skull_tile.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_OBJECT_SKULL_TILE_HPP
#define HEADER_SUPERTUX_OBJECT_SKULL_TILE_HPP

#include "object/moving_sprite.hpp"

/** A tile that starts falling down if tux stands to long on it */
class SkullTile : public MovingSprite
{
public:
  SkullTile(const ReaderMapping& lisp);
  std::string get_class() const {
    return "skulltile";
  }

  HitResponse collision(GameObject& other, const CollisionHit& hit);
  void update(float elapsed_time);
  void draw(DrawingContext& context);

private:
  Physic physic;
  Timer timer;
  bool hit;
  bool falling;
};

#endif // HEADER_SUPERTUX_OBJECT_SKULL_TILE_HPP

// EOF
