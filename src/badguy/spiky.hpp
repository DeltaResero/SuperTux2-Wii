// src/badguy/spiky.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_BADGUY_SPIKY_HPP
#define HEADER_SUPERTUX_BADGUY_SPIKY_HPP

#include "badguy/walking_badguy.hpp"

class Spiky : public WalkingBadguy
{
public:
  Spiky(const ReaderMapping& reader);

  bool is_freezable() const;
  bool is_flammable() const;
  std::string get_class() const {
    return "spiky";
  }
  std::string get_display_name() const {
    return _("Spiky");
  }

private:
};

#endif // HEADER_SUPERTUX_BADGUY_SPIKY_HPP

// EOF
