// src/object/firefly.cpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
//  SuperTux
//  Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "object/firefly.hpp"
#include <numbers>

#include <math.h>
#include <cmath>
#include <memory>

#include "audio/sound_manager.hpp"
#include "math/random_generator.hpp"
#include "object/player.hpp"
#include "object/sprite_particle.hpp"
#include "supertux/game_session.hpp"
#include "supertux/object_factory.hpp"
#include "supertux/sector.hpp"
#include "util/reader_mapping.hpp"

/* A reset point is where Tux's head goes. The sector lowers a small Tux by
   the difference between the two heights, so either size lands feet first on
   the same spot, and putting that spot at the bell's foot is what stands him
   on the ground beside it. A bell hung clear of the ground already reads
   right and must be left alone, or he arrives inside whatever is overhead. */
static const float RESET_POINT_GROUND_REACH = 16.0f;

Firefly::Firefly(const ReaderMapping& lisp) :
   MovingSprite(lisp, "images/objects/resetpoints/default-resetpoint.sprite", LAYER_TILES, COLGROUP_TOUCHABLE),
   activated(false),
   initial_position()
{
  initial_position = get_pos();
  if( !lisp.get( "sprite", sprite_name ) ){
    reactivate();
    return;
  }
  if (sprite_name.empty()) {
    sprite_name = "images/objects/resetpoints/default-resetpoint.sprite";
    reactivate();
    return;
  }
  //Replace sprite
  sprite = SpriteManager::current()->create( sprite_name );
  bbox.set_size(sprite->get_current_hitbox_width(), sprite->get_current_hitbox_height());
  reactivate();

  //Load sound
    if( sprite_name.find("vbell", 0) == std::string::npos ) {
      SoundManager::current()->preload("sounds/savebell_low.wav");
    }
    else {
      SoundManager::current()->preload("sounds/savebell2.wav");
    }
}

Vector
Firefly::reset_point() const
{
  return Vector(initial_position.x, bbox.get_bottom() - BIG_TUX_HEIGHT);
}

void
Firefly::reactivate()
{
  if (!GameSession::current()) {
    return;
  }
  if(!GameSession::current()->get_reset_point_sectorname().empty() &&
     (GameSession::current()->get_reset_point_pos() == initial_position ||
      GameSession::current()->get_reset_point_pos() == reset_point())) {
    // TODO: && GameSession::current()->get_reset_point_sectorname() ==  <sector this firefly is in>
    // GameSession::current()->get_current_sector()->get_name() is not yet initialized.
    // Worst case a resetpoint in a different sector at the same position as the real
    // resetpoint the player is spawning is set to ringing, too. Until we can check the sector, too, dont set
    // activated = true; here.
    sprite->set_action("ringing");
  }
}

HitResponse
Firefly::collision(GameObject& other, const CollisionHit& )
{
  // If the bell is already activated, don't ring it again!
  if(activated || sprite->get_action() == "ringing")
    return ABORT_MOVE;

  auto player = dynamic_cast<Player*> (&other);
  if(player) {
    activated = true;
    // spawn some particles
    // TODO: provide convenience function in MovingSprite or MovingObject?
    for (int i = 0; i < 5; i++) {
      Vector ppos = bbox.get_middle();
      float angle = graphicsRandom.randf(-(std::numbers::pi_v<float> / 2), (std::numbers::pi_v<float> / 2));
      float velocity = graphicsRandom.randf(450, 900);
      float vx = sin(angle)*velocity;
      float vy = -cos(angle)*velocity;
      Vector pspeed = Vector(vx, vy);
      Vector paccel = Vector(0, 1000);
      Sector::current()->add_object(std::make_shared<SpriteParticle>("images/objects/particles/reset.sprite", "default", ppos, ANCHOR_MIDDLE, pspeed, paccel, LAYER_OBJECTS-1));
    }

    if( sprite_name.find("vbell", 0) == std::string::npos ) {
      SoundManager::current()->play("sounds/savebell2.wav");
    }
    else {
      SoundManager::current()->play("sounds/savebell_low.wav");
    }

    sprite->set_action("ringing");
    const Rectf underfoot(bbox.get_left(), bbox.get_bottom(),
                          bbox.get_right(), bbox.get_bottom() + RESET_POINT_GROUND_REACH);
    GameSession::current()->set_reset_point(Sector::current()->get_name(),
                                            Sector::current()->is_free_of_tiles(underfoot)
                                            ? initial_position : reset_point());
  }

  return ABORT_MOVE;
}

/* EOF */
