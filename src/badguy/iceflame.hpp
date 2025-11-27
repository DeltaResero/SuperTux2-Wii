// src/badguy/iceflame.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux - Iceflame a flame-like enemy that can be killed with fireballs
// Copyright (C) 2013 LMH <lmh.0013@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_BADGUY_ICEFLAME_HPP
#define HEADER_SUPERTUX_BADGUY_ICEFLAME_HPP

#include "badguy/flame.hpp"

class Iceflame : public Flame
{
public:
  Iceflame(const ReaderMapping& reader);
  Iceflame(const Iceflame& iceflame);

  void active_update(float elapsed_time);

  void ignite();
  bool is_flammable() const;
  bool is_freezable() const;
  std::string get_class() const {
    return "iceflame";
  }

  std::string get_display_name() const {
    return _("Ice flame");
  }
};

#endif // HEADER_SUPERTUX_BADGUY_ICEFLAME_HPP

// EOF
