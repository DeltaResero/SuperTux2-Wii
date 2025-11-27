// src/object/star.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_OBJECT_STAR_HPP
#define HEADER_SUPERTUX_OBJECT_STAR_HPP

#include "object/moving_sprite.hpp"

class Star : public MovingSprite
{
public:
  Star(const Vector& pos, Direction direction = RIGHT);

  virtual void update(float elapsed_time);
  virtual void draw(DrawingContext& context);
  virtual void collision_solid(const CollisionHit& hit);
  virtual HitResponse collision(GameObject& other, const CollisionHit& hit);
  virtual bool do_save() const {
    return false;
  }

private:
  Physic physic;
  Color light;
  SpritePtr lightsprite;
};

#endif // HEADER_SUPERTUX_OBJECT_STAR_HPP

// EOF
