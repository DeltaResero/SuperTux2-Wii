// src/badguy/angrystone.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux - A spiked block that charges towards the player
// Copyright (C) 2006 Christoph Sommer <christoph.sommer@2006.expires.deltadevelopment.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_BADGUY_ANGRYSTONE_HPP
#define HEADER_SUPERTUX_BADGUY_ANGRYSTONE_HPP

#include "badguy/badguy.hpp"

class AngryStone : public BadGuy
{
public:
  AngryStone(const ReaderMapping& reader);

  void collision_solid(const CollisionHit& hit);
  HitResponse collision_badguy(BadGuy& badguy, const CollisionHit& hit);
  void active_update(float elapsed_time);
  void kill_fall();
  bool is_freezable() const;
  bool is_flammable() const;
  std::string get_class() const {
    return "angrystone";
  }
  std::string get_display_name() const {
    return _("Angry stone");
  }

protected:
  enum AngryStoneState {
    IDLE,
    CHARGING,
    ATTACKING,
    RECOVERING
  };

private:
  Vector attackDirection;  /**< 1-normalized vector of current attack direction */
  Vector oldWallDirection; /**< if wall was hit during last attack: 1-normalized vector of last attack direction, (0,0) otherwise */
  Timer timer;
  AngryStoneState state;
};

#endif // HEADER_SUPERTUX_BADGUY_ANGRYSTONE_HPP

// EOF
