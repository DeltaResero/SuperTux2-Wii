// src/badguy/ghostflame.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux - Ghostflame a flame-like enemy that cannot be killed
// Copyright (C) 2013 LMH <lmh.0013@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_BADGUY_GHOSTFLAME_HPP
#define HEADER_SUPERTUX_BADGUY_GHOSTFLAME_HPP

#include "badguy/flame.hpp"

class Ghostflame : public Flame
{
public:
  Ghostflame(const ReaderMapping& reader);
  Ghostflame(const Ghostflame& ghostflame);

  bool is_flammable() const;
  bool is_freezable() const;
  std::string get_class() const {
    return "ghostflame";
  }

  std::string get_display_name() const {
    return _("Ghost flame");
  }
};

#endif // HEADER_SUPERTUX_BADGUY_GHOSTFLAME_HPP

// EOF
