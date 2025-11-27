// src/badguy/smartball.cpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux - Smart Snowball
// Copyright (C) 2008 Wolfgang Becker <uafr@gmx.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#include "badguy/smartball.hpp"

#include "audio/sound_manager.hpp"
#include "sprite/sprite.hpp"
#include "supertux/object_factory.hpp"
#include "supertux/sector.hpp"

SmartBall::SmartBall(const ReaderMapping& reader)
  : WalkingBadguy(reader, "images/creatures/snowball/smart-snowball.sprite", "left", "right")
{
  walk_speed = 80;
  max_drop_height = 16;
}

SmartBall::SmartBall(const Vector& pos, Direction d)
  : WalkingBadguy(pos, d, "images/creatures/snowball/smart-snowball.sprite", "left", "right")
{
  walk_speed = 80;
  max_drop_height = 16;
}

bool
SmartBall::collision_squished(GameObject& object)
{
  sprite->set_action(dir == LEFT ? "squished-left" : "squished-right");
  kill_squished(object);
  return true;
}

// EOF
