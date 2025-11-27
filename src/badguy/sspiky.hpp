// src/badguy/sspiky.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_BADGUY_SSPIKY_HPP
#define HEADER_SUPERTUX_BADGUY_SSPIKY_HPP

#include "badguy/walking_badguy.hpp"

class SSpiky : public WalkingBadguy
{
public:
  SSpiky(const ReaderMapping& reader);

  void initialize();
  void collision_solid(const CollisionHit& hit);
  HitResponse collision_badguy(BadGuy& badguy, const CollisionHit& hit);
  void active_update(float elapsed_time);

  void freeze();
  bool is_freezable() const;
  bool is_flammable() const;
  std::string get_class() const {
    return "sspiky";
  }
  std::string get_display_name() const {
    return _("Sleeping spiky");
  }

protected:
  enum SSpikyState {
    SSPIKY_SLEEPING,
    SSPIKY_WAKING,
    SSPIKY_WALKING
  };
  SSpikyState state;
};

#endif // HEADER_SUPERTUX_BADGUY_SSPIKY_HPP

// EOF
