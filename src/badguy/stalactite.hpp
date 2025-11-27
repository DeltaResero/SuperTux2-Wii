// src/badguy/stalactite.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_BADGUY_STALACTITE_HPP
#define HEADER_SUPERTUX_BADGUY_STALACTITE_HPP

#include "badguy/badguy.hpp"

class Stalactite : public BadGuy
{
public:
  Stalactite(const ReaderMapping& reader);

  void active_update(float elapsed_time);
  void collision_solid(const CollisionHit& hit);
  HitResponse collision_player(Player& player, const CollisionHit& hit);
  HitResponse collision_badguy(BadGuy& other, const CollisionHit& hit);
  HitResponse collision_bullet(Bullet& bullet, const CollisionHit& );

  void kill_fall();
  void draw(DrawingContext& context);
  void deactivate();

  void squish();
  std::string get_class() const {
    return "stalactite";
  }
  std::string get_display_name() const {
    return _("Stalactite");
  }

protected:
  enum StalactiteState {
    STALACTITE_HANGING,
    STALACTITE_SHAKING,
    STALACTITE_FALLING,
    STALACTITE_SQUISHED
  };

protected:
  Timer timer;
  StalactiteState state;
  Vector shake_delta;
};

#endif // HEADER_SUPERTUX_BADGUY_STALACTITE_HPP

// EOF
