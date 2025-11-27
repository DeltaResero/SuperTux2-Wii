// src/badguy/zeekling.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux - flyer that swoops down when she spots the player
// Copyright (C) 2005 Matthias Braun <matze@braunis.de>
// Copyright (C) 2006 Christoph Sommer <christoph.sommer@2006.expires.deltadevelopment.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_BADGUY_ZEEKLING_HPP
#define HEADER_SUPERTUX_BADGUY_ZEEKLING_HPP

#include "badguy/badguy.hpp"

class Zeekling : public BadGuy
{
public:
  Zeekling(const ReaderMapping& reader);
  Zeekling(const Vector& pos, Direction d);

  void initialize();
  void collision_solid(const CollisionHit& hit);
  void active_update(float elapsed_time);

  void freeze();
  void unfreeze();
  bool is_freezable() const;
  std::string get_class() const {
    return "zeekling";
  }
  std::string get_display_name() const {
    return _("Zeekling");
  }

private:
  bool collision_squished(GameObject& object);
  bool should_we_dive();
  void onBumpHorizontal();
  void onBumpVertical();

private:
  enum ZeeklingState {
    FLYING,
    DIVING,
    CLIMBING
  };

private:
  float speed;
  Timer diveRecoverTimer;
  ZeeklingState state;
  const MovingObject* last_player; /**< last player we tracked */
  Vector last_player_pos; /**< position we last spotted the player at */
  Vector last_self_pos; /**< position we last were at */

private:
  Zeekling(const Zeekling&);
  Zeekling& operator=(const Zeekling&);
};

#endif // HEADER_SUPERTUX_BADGUY_ZEEKLING_HPP

// EOF
