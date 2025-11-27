// src/badguy/walkingleaf.cpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux - Walking Leaf
// Copyright (C) 2006 Wolfgang Becker <uafr@gmx.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#include "badguy/walkingleaf.hpp"

#include "object/sprite_particle.hpp"
#include "supertux/object_factory.hpp"

WalkingLeaf::WalkingLeaf(const ReaderMapping& reader) :
  WalkingBadguy(reader, "images/creatures/walkingleaf/walkingleaf.sprite", "left", "right")
{
  walk_speed = 60;
  max_drop_height = 16;
}

WalkingLeaf::WalkingLeaf(const Vector& pos, Direction d)
  : WalkingBadguy(pos, d, "images/creatures/walkingleaf/walkingleaf.sprite", "left", "right")
{
  walk_speed = 60;
  max_drop_height = 16;
}

bool
WalkingLeaf::collision_squished(GameObject& object)
{
  sprite->set_action(dir == LEFT ? "squished-left" : "squished-right");
  kill_squished(object);
  return true;
}

bool
WalkingLeaf::is_freezable() const
{
  return true;
}

// EOF
