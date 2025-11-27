// src/object/hurting_platform.cpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux - Hurting Platform
// Copyright (C) 2006 Christoph Sommer <christoph.sommer@2006.expires.deltadevelopment.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#include "object/hurting_platform.hpp"

#include "badguy/badguy.hpp"
#include "object/player.hpp"
#include "supertux/object_factory.hpp"

HurtingPlatform::HurtingPlatform(const ReaderMapping& reader)
  : Platform(reader, "images/objects/sawblade/sawblade.sprite")
{
  set_group(COLGROUP_TOUCHABLE);
}

HitResponse
HurtingPlatform::collision(GameObject& other, const CollisionHit& )
{
  auto player = dynamic_cast<Player*>(&other);
  if (player) {
    if(player->is_invincible()) {
      return ABORT_MOVE;
    }
    player->kill(false);
  }
  auto badguy = dynamic_cast<BadGuy*>(&other);
  if (badguy) {
    badguy->kill_fall();
  }

  return FORCE_MOVE;
}

// EOF
