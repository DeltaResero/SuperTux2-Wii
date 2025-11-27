// src/badguy/short_fuse.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
// Copyright (C) 2010 Florian Forster <supertux at octo.it>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_BADGUY_SHORT_FUSE_HPP
#define HEADER_SUPERTUX_BADGUY_SHORT_FUSE_HPP

#include "badguy/walking_badguy.hpp"

class ShortFuse : public WalkingBadguy
{
public:
  ShortFuse(const ReaderMapping& reader);
  std::string get_class() const {
    return "short_fuse";
  }
  std::string get_display_name() const {
    return _("Short fuse");
  }

protected:
  HitResponse collision_player (Player& player, const CollisionHit& hit);
  HitResponse collision_bullet (Bullet& bullet, const CollisionHit& );
  bool collision_squished (GameObject& object);
  void kill_fall();
  void ignite();
  void explode();
};

#endif // HEADER_SUPERTUX_BADGUY_SHORT_FUSE_HPP

// EOF
