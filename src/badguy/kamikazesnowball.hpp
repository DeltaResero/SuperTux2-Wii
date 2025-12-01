// src/badguy/kamikazesnowball.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2008 Wolfgang Becker <uafr@gmx.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_BADGUY_KAMIKAZESNOWBALL_HPP
#define HEADER_SUPERTUX_BADGUY_KAMIKAZESNOWBALL_HPP

#include "badguy/badguy.hpp"

class KamikazeSnowball : public BadGuy
{
public:
  KamikazeSnowball(const ReaderMapping& reader);
  KamikazeSnowball(const Vector& pos, Direction d);

  void initialize();
  void collision_solid(const CollisionHit& hit);
  virtual std::string get_class() const {
    return "kamikazesnowball";
  }
  virtual std::string get_display_name() const {
    return _("Kamikaze snowball");
  }

protected:
  bool collision_squished(GameObject& object);
  HitResponse collision_player(Player& player, const CollisionHit& hit);
  void kill_collision();
};

class LeafShot : public KamikazeSnowball
{
  public:
    LeafShot(const ReaderMapping& reader);

    void initialize();
    bool is_freezable() const;
    std::string get_class() const {
      return "leafshot";
    }
    std::string get_display_name() const {
      return _("Leaf Shot");
    }
};

#endif // HEADER_SUPERTUX_BADGUY_KAMIKAZESNOWBALL_HPP

// EOF
