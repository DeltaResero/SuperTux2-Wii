// src/badguy/walkingleaf.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux - Walking Leaf
// Copyright (C) 2006 Wolfgang Becker <uafr@gmx.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_BADGUY_WALKINGLEAF_HPP
#define HEADER_SUPERTUX_BADGUY_WALKINGLEAF_HPP

#include "badguy/walking_badguy.hpp"

/*
 * Easy to kill badguy that does not jump down from it's ledge.
 */
class WalkingLeaf : public WalkingBadguy
{
public:
  WalkingLeaf(const ReaderMapping& reader);
  WalkingLeaf(const Vector& pos, Direction d);

  bool is_freezable() const;
  std::string get_class() const {
    return "walkingleaf";
  }
  std::string get_display_name() const {
    return _("Walking leaf");
  }

protected:
  bool collision_squished(GameObject& object);

};

#endif // HEADER_SUPERTUX_BADGUY_WALKINGLEAF_HPP

// EOF
