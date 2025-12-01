// src/badguy/crystallo.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux - Crystallo
// Copyright (C) 2008 Wolfgang Becker <uafr@gmx.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_BADGUY_CRYSTALLO_HPP
#define HEADER_SUPERTUX_BADGUY_CRYSTALLO_HPP

#include "badguy/walking_badguy.hpp"

/*
 * Basic badguy, patrols around a fixed position.
 */
class Crystallo : public WalkingBadguy
{
public:
  Crystallo(const ReaderMapping& reader);
  Crystallo(const Vector& pos, Direction d);
  std::string get_class() const {
    return "crystallo";
  }
  std::string get_display_name() const {
    return _("Crystallo");
  }

  void active_update(float elapsed_time);
  bool is_flammable() const;

protected:
  bool collision_squished(GameObject& object);

private:
  float radius;
};

#endif // HEADER_SUPERTUX_BADGUY_CRYSTALLO_HPP

// EOF
