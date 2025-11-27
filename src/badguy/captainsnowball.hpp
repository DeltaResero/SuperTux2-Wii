// src/badguy/captainsnowball.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2008 Wolfgang Becker <uafr@gmx.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_BADGUY_CAPTAINSNOWBALL_HPP
#define HEADER_SUPERTUX_BADGUY_CAPTAINSNOWBALL_HPP

#include "badguy/walking_badguy.hpp"

class CaptainSnowball : public WalkingBadguy
{
public:
  CaptainSnowball(const ReaderMapping& reader);
  CaptainSnowball(const Vector& pos, Direction d);

  virtual void active_update(float elapsed_time);
  void collision_solid(const CollisionHit& hit);

  bool might_climb(int width, int height) const;
  std::string get_class() const {
    return "captainsnowball";
  }
  std::string get_display_name() const {
    return _("Captain snowball");
  }

protected:
  bool collision_squished(GameObject& object);

};

#endif // HEADER_SUPERTUX_BADGUY_CAPTAINSNOWBALL_HPP

// EOF
