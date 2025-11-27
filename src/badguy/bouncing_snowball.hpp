// src/badguy/bouncing_snowball.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_BADGUY_BOUNCING_SNOWBALL_HPP
#define HEADER_SUPERTUX_BADGUY_BOUNCING_SNOWBALL_HPP

#include "badguy/badguy.hpp"

class BouncingSnowball : public BadGuy
{
public:
  BouncingSnowball(const ReaderMapping& reader);
  BouncingSnowball(const Vector& pos, Direction d);

  void initialize();
  void collision_solid(const CollisionHit& hit);
  HitResponse collision_badguy(BadGuy& badguy, const CollisionHit& hit);
  std::string get_class() const {
    return "bouncingsnowball";
  }
  std::string get_display_name() const {
    return _("Bouncing Snowball");
  }

  void after_editor_set();

protected:
  bool collision_squished(GameObject& object);
};

#endif // HEADER_SUPERTUX_BADGUY_BOUNCING_SNOWBALL_HPP

// EOF
