// src/object/rock.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_OBJECT_ROCK_HPP
#define HEADER_SUPERTUX_OBJECT_ROCK_HPP

#include "object/moving_sprite.hpp"
#include "object/portable.hpp"
#include "supertux/physic.hpp"

class Rock : public MovingSprite,
             public Portable
{
public:
  Rock(const Vector& pos, const std::string& spritename);
  Rock(const ReaderMapping& reader);
  Rock(const ReaderMapping& reader, const std::string& spritename);

  void collision_solid(const CollisionHit& hit);
  HitResponse collision(GameObject& other, const CollisionHit& hit);
  void update(float elapsed_time);

  void grab(MovingObject& object, const Vector& pos, Direction dir);
  void ungrab(MovingObject& object, Direction dir);
  std::string get_class() const {
    return "rock";
  }
  std::string get_display_name() const {
    return _("Rock");
  }

protected:
  Physic physic;
  bool on_ground;
  bool grabbed;
  Vector last_movement;
};

#endif // HEADER_SUPERTUX_OBJECT_ROCK_HPP

// EOF
