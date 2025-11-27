// src/badguy/snowball.cpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#include "badguy/snowball.hpp"

#include "sprite/sprite.hpp"
#include "supertux/object_factory.hpp"

SnowBall::SnowBall(const ReaderMapping& reader)
  : WalkingBadguy(reader, "images/creatures/snowball/snowball.sprite", "left", "right")
{
  walk_speed = 80;
}

SnowBall::SnowBall(const Vector& pos, Direction d, std::string script)
  : WalkingBadguy(pos, d, "images/creatures/snowball/snowball.sprite", "left", "right")
{
  walk_speed = 80;
  dead_script = script;
}

bool
SnowBall::collision_squished(GameObject& object)
{
  sprite->set_action(dir == LEFT ? "squished-left" : "squished-right");
  kill_squished(object);
  return true;
}

// EOF
