// src/badguy/mrbomb.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_BADGUY_MRBOMB_HPP
#define HEADER_SUPERTUX_BADGUY_MRBOMB_HPP

#include "badguy/walking_badguy.hpp"

class MrBomb : public WalkingBadguy,
               public Portable
{
public:
  MrBomb(const ReaderMapping& reader);
  MrBomb(const Vector& pos, Direction d);

  void kill_fall();
  void ignite();
  HitResponse collision(GameObject& object, const CollisionHit& hit);
  HitResponse collision_player(Player& player, const CollisionHit& hit);

  void active_update(float elapsed_time);

  void grab(MovingObject& object, const Vector& pos, Direction dir);
  void ungrab(MovingObject& object, Direction dir);
  bool is_portable() const;

  bool is_freezable() const;
  std::string get_class() const {
    return "mrbomb";
  }
  std::string get_display_name() const {
    return _("Mr. Bomb");
  }

protected:
  bool collision_squished(GameObject& object);

private:
  bool grabbed;
};

#endif // HEADER_SUPERTUX_BADGUY_MRBOMB_HPP

// EOF
