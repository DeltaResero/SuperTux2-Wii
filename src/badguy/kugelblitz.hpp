// src/badguy/kugelblitz.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_BADGUY_KUGELBLITZ_HPP
#define HEADER_SUPERTUX_BADGUY_KUGELBLITZ_HPP

#include "badguy/badguy.hpp"

class Kugelblitz : public BadGuy
{
public:
  Kugelblitz(const ReaderMapping& reader);

  void initialize();
  HitResponse collision_badguy(BadGuy& other, const CollisionHit& hit);
  void collision_solid(const CollisionHit& hit);
  HitResponse collision_player(Player& player, const CollisionHit& hit);

  void active_update(float);
  void kill_fall();
  void explode();

  bool is_flammable() const;

  virtual void draw(DrawingContext& context);
  std::string get_class() const {
    return "kugelblitz";
  }
  std::string get_display_name() const {
    return _("Kugelblitz");
  }

private:
  void try_activate();
  HitResponse hit(const CollisionHit& hit);

private:
  Vector pos_groundhit;
  bool groundhit_pos_set;
  bool dying;
  Timer movement_timer;
  Timer lifetime;
  int direction;
  Color light;
  SpritePtr lightsprite;
};

#endif // HEADER_SUPERTUX_BADGUY_KUGELBLITZ_HPP

// EOF
