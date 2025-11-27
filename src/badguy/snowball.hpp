// src/badguy/snowball.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_BADGUY_SNOWBALL_HPP
#define HEADER_SUPERTUX_BADGUY_SNOWBALL_HPP

#include "badguy/walking_badguy.hpp"

class SnowBall : public WalkingBadguy
{
public:
  SnowBall(const ReaderMapping& reader);
  SnowBall(const Vector& pos, Direction d, std::string script);
  std::string get_class() const {
    return "snowball";
  }
  std::string get_display_name() const {
    return _("Snowball");
  }

protected:
  bool collision_squished(GameObject& object);

};

#endif // HEADER_SUPERTUX_BADGUY_SNOWBALL_HPP

// EOF
