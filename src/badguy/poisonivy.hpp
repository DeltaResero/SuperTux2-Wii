// src/badguy/poisonivy.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_BADGUY_POISONIVY_HPP
#define HEADER_SUPERTUX_BADGUY_POISONIVY_HPP

#include "badguy/walking_badguy.hpp"

class PoisonIvy : public WalkingBadguy
{
public:
  PoisonIvy(const ReaderMapping& reader);
  PoisonIvy(const Vector& pos, Direction d);

  bool is_freezable() const;
  std::string get_class() const {
    return "poisonivy";
  }
  std::string get_display_name() const {
    return _("Poisonous ivy");
  }

protected:
  bool collision_squished(GameObject& object);

};

#endif // HEADER_SUPERTUX_BADGUY_POISONIVY_HPP

// EOF
