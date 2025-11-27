// src/badguy/skullyhop.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux - A Hopping Skull
// Copyright (C) 2006 Christoph Sommer <christoph.sommer@2006.expires.deltadevelopment.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_BADGUY_SKULLYHOP_HPP
#define HEADER_SUPERTUX_BADGUY_SKULLYHOP_HPP

#include "badguy/badguy.hpp"

/**
 * Badguy "SkullyHop" - A Hopping Skull
 */
class SkullyHop : public BadGuy
{
public:
  SkullyHop(const ReaderMapping& reader);
  SkullyHop(const Vector& pos, Direction d);

  void initialize();
  void collision_solid(const CollisionHit& hit);
  HitResponse collision_badguy(BadGuy& badguy, const CollisionHit& hit);
  bool collision_squished(GameObject& object);
  void active_update(float elapsed_time);

  void unfreeze();
  bool is_freezable() const;
  std::string get_class() const {
    return "skullyhop";
  }
  std::string get_display_name() const {
    return _("Skully hop");
  }

  void after_editor_set();

private:
  enum SkullyHopState {
    STANDING,
    CHARGING,
    JUMPING
  };

private:
  void set_state(SkullyHopState newState);

private:
  Timer recover_timer;
  SkullyHopState state;
};

#endif // HEADER_SUPERTUX_BADGUY_SKULLYHOP_HPP

// EOF
