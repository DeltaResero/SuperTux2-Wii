// src/badguy/stumpy.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_BADGUY_STUMPY_HPP
#define HEADER_SUPERTUX_BADGUY_STUMPY_HPP

#include "badguy/walking_badguy.hpp"

class Stumpy : public WalkingBadguy
{
public:
  Stumpy(const ReaderMapping& reader);
  Stumpy(const Vector& pos, Direction d);

  void initialize();
  void active_update(float elapsed_time);
  void collision_solid(const CollisionHit& hit);
  HitResponse collision_badguy(BadGuy& badguy, const CollisionHit& hit);

  bool is_freezable() const;
  std::string get_class() const {
    return "stumpy";
  }
  std::string get_display_name() const {
    return _("Stumpy");
  }

protected:
  enum MyState {
    STATE_INVINCIBLE, STATE_NORMAL
  };

protected:
  bool collision_squished(GameObject& object);

private:
  MyState mystate;
  Timer   invincible_timer;
};

#endif // HEADER_SUPERTUX_BADGUY_STUMPY_HPP

// EOF
