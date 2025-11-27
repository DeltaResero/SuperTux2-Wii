// src/badguy/snowman.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2010 Ingo Ruhnke <grumbel@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_BADGUY_SNOWMAN_HPP
#define HEADER_SUPERTUX_BADGUY_SNOWMAN_HPP

#include "badguy/walking_badguy.hpp"

class Snowman : public WalkingBadguy
{
public:
  Snowman(const ReaderMapping& reader);
  Snowman(const Vector& pos, Direction d);
  std::string get_class() const {
    return "snowman";
  }
  std::string get_display_name() const {
    return _("Snowman");
  }

protected:
  void loose_head();
  virtual HitResponse collision_bullet(Bullet& bullet, const CollisionHit& hit);
  bool collision_squished(GameObject& object);

};

#endif // HEADER_SUPERTUX_BADGUY_SNOWMAN_HPP

// EOF
