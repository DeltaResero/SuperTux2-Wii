// src/badguy/walking_candle.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2015 Hume2 <teratux.mail@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_BADGUY_WALKING_CANDLE_HPP
#define HEADER_SUPERTUX_BADGUY_WALKING_CANDLE_HPP

#include "badguy/walking_badguy.hpp"

class Color;

class WalkingCandle : public WalkingBadguy
{
public:
  WalkingCandle(const ReaderMapping& reader);

  bool is_freezable() const;
  bool is_flammable() const;

  void freeze();
  void unfreeze();

  HitResponse collision(GameObject& other, const CollisionHit& hit);

  void kill_fall() { };

  std::string get_class() const {
    return "mrcandle";
  }
  std::string get_display_name() const {
    return _("Mr. Candle");
  }

private:

  Color lightcolor;
};

#endif // HEADER_SUPERTUX_BADGUY_WALKING_CANDLE_HPP

// EOF
