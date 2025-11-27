// src/object/firefly.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_OBJECT_FIREFLY_HPP
#define HEADER_SUPERTUX_OBJECT_FIREFLY_HPP

#include "object/moving_sprite.hpp"

/**
 * A Firefly: When tux touches it, it begins buzzing and you will respawn at this
 * position.
 */
class Firefly : public MovingSprite
{
public:
  Firefly(const ReaderMapping& lisp);

  HitResponse collision(GameObject& other, const CollisionHit& hit);
  std::string get_class() const {
    return "firefly";
  }
  std::string get_display_name() const {
    return _("Reset point");
  }

private:
  bool activated;
  Vector initial_position; /**< position as in level file. This is where Tux will have to respawn, as the level is reset every time */
  void reactivate();
};

#endif // HEADER_SUPERTUX_OBJECT_FIREFLY_HPP

// EOF
