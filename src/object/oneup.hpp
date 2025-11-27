// src/object/oneup.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_OBJECT_ONEUP_HPP
#define HEADER_SUPERTUX_OBJECT_ONEUP_HPP

#include "object/moving_sprite.hpp"
#include "supertux/direction.hpp"
#include "supertux/physic.hpp"

class OneUp : public MovingSprite
{
public:
  OneUp(const Vector& pos, Direction direction = RIGHT);
  virtual bool do_save() const {
    return false;
  }

  virtual void update(float elapsed_time);
  virtual HitResponse collision(GameObject& other, const CollisionHit& hit);

private:
  Physic physic;
};

#endif // HEADER_SUPERTUX_OBJECT_ONEUP_HPP

// EOF
