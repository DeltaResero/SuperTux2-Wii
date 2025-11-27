// src/badguy/goldbomb.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux - a bomb that throws up coins when exploding
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
// Copyright (C) 2013 LMH <lmh.0013@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_BADGUY_GOLDBOMB_HPP
#define HEADER_SUPERTUX_BADGUY_GOLDBOMB_HPP

#include "audio/sound_source.hpp"
#include "badguy/walking_badguy.hpp"
#include "object/portable.hpp"

class GoldBomb : public WalkingBadguy, public Portable
{
public:
  GoldBomb(const ReaderMapping& reader);

  void collision_solid(const CollisionHit& hit);
  HitResponse collision(GameObject& object, const CollisionHit& hit);
  HitResponse collision_player(Player& player, const CollisionHit& hit);
  HitResponse collision_badguy(BadGuy& badguy, const CollisionHit& hit);

  void active_update(float elapsed_time);

  void grab(MovingObject& object, const Vector& pos, Direction dir);
  void ungrab(MovingObject& object, Direction dir);
  bool is_portable() const;

  void freeze();
  bool is_freezable() const;

  void kill_fall();
  void ignite();
  std::string get_class() const {
    return "goldbomb";
  }
  std::string get_display_name() const {
    return _("Golden bomb");
  }

  void stop_looping_sounds();
  void play_looping_sounds();

protected:
  bool collision_squished(GameObject& object);

private:
  GoldBomb(const GoldBomb&);
  GoldBomb& operator=(const GoldBomb&);
  enum Ticking_State {
    STATE_NORMAL,
    STATE_TICKING
  };

  Ticking_State tstate;
  bool grabbed;
  MovingObject* grabber;

  std::unique_ptr<SoundSource> ticking;
};

#endif // HEADER_SUPERTUX_BADGUY_GOLDBOMB_HPP

// EOF
