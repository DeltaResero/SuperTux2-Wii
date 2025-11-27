// src/badguy/jumpy.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_BADGUY_JUMPY_HPP
#define HEADER_SUPERTUX_BADGUY_JUMPY_HPP

#include "badguy/badguy.hpp"

class Jumpy : public BadGuy
{
public:
  Jumpy(const ReaderMapping& reader);

  void collision_solid(const CollisionHit& hit);
  HitResponse collision_badguy(BadGuy& other, const CollisionHit& hit);

  void active_update(float);

  void freeze();
  bool is_freezable() const;
  bool is_flammable() const;
  std::string get_class() const {
    return "jumpy";
  }
  std::string get_display_name() const {
    return _("Jumpy");
  }

private:
  HitResponse hit(const CollisionHit& hit);

private:
  Vector pos_groundhit;
  bool groundhit_pos_set;
};

#endif // HEADER_SUPERTUX_BADGUY_JUMPY_HPP

// EOF
