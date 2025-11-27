// src/badguy/mole.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux - Mole Badguy
// Copyright (C) 2006 Christoph Sommer <christoph.sommer@2006.expires.deltadevelopment.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_BADGUY_MOLE_HPP
#define HEADER_SUPERTUX_BADGUY_MOLE_HPP

#include "badguy/badguy.hpp"

class Mole : public BadGuy
{
public:
  Mole(const ReaderMapping& );
  Mole(const Vector& pos);

  void kill_fall();
  HitResponse collision_badguy(BadGuy& , const CollisionHit& );
  bool collision_squished(GameObject& object);

  void activate();
  void active_update(float);

  bool is_freezable() const;

  virtual void ignite();

  std::string get_class() const {
    return "mole";
  }
  std::string get_display_name() const {
    return _("Mole");
  }

private:
  enum MoleState {
    PRE_THROWING,
    THROWING,
    POST_THROWING,
    PEEKING,
    DEAD,
    BURNING
  };

private:
  void set_state(MoleState new_state);
  void throw_rock();

private:
  MoleState state;
  Timer timer;
  Timer throw_timer;
};

#endif // HEADER_SUPERTUX_BADGUY_MOLE_HPP

// EOF
