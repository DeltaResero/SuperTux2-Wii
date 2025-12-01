// src/trigger/switch.cpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux - Switch Trigger
// Copyright (C) 2006 Christoph Sommer <christoph.sommer@2006.expires.deltadevelopment.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#include <config.h>
#include <stdexcept>

#include "audio/sound_manager.hpp"
#include "sprite/sprite.hpp"
#include "sprite/sprite_manager.hpp"
#include "supertux/object_factory.hpp"
#include "supertux/sector.hpp"
#include "trigger/switch.hpp"
#include "util/gettext.hpp"

#include <sstream>

namespace {
const std::string SWITCH_SOUND = "sounds/switch.ogg";
}

Switch::Switch(const ReaderMapping& reader) :
  sprite_name(),
  sprite(),
  script(),
  off_script(),
  state(OFF),
  bistable()
{
  if (!reader.get("x", bbox.p1.x)) throw std::runtime_error("no x position set");
  if (!reader.get("y", bbox.p1.y)) throw std::runtime_error("no y position set");
  if (!reader.get("sprite", sprite_name)) sprite_name = "images/objects/switch/left.sprite";
  sprite = SpriteManager::current()->create(sprite_name);
  bbox.set_size(sprite->get_current_hitbox_width(), sprite->get_current_hitbox_height());

  reader.get("script", script);
  bistable = reader.get("off-script", off_script);

  SoundManager::current()->preload( SWITCH_SOUND );
}

Switch::~Switch()
{
}

void
Switch::update(float )
{
  switch (state) {
    case OFF:
      break;
    case TURN_ON:
      if(sprite->animation_done()) {
        std::istringstream stream(script);
        std::ostringstream location;
        location << "switch" << bbox.p1;
        Sector::current()->run_script(stream, location.str());

        sprite->set_action("on", 1);
        state = ON;
      }
      break;
    case ON:
      if(sprite->animation_done() && !bistable) {
        sprite->set_action("turnoff", 1);
        state = TURN_OFF;
      }
      break;
    case TURN_OFF:
      if(sprite->animation_done()) {
        if (bistable) {
          std::istringstream stream(off_script);
          std::ostringstream location;
          location << "switch" << bbox.p1;
          Sector::current()->run_script(stream, location.str());
        }

        sprite->set_action("off");
        state = OFF;
      }
      break;
  }
}

void
Switch::draw(DrawingContext& context)
{
  sprite->draw(context, bbox.p1, LAYER_TILES);
}

void
Switch::event(Player& , EventType type)
{
  if(type != EVENT_ACTIVATE) return;

  switch (state) {
    case OFF:
      sprite->set_action("turnon", 1);
      SoundManager::current()->play( SWITCH_SOUND );
      state = TURN_ON;
      break;
    case TURN_ON:
      break;
    case ON:
      if (bistable) {
        sprite->set_action("turnoff", 1);
        SoundManager::current()->play( SWITCH_SOUND );
        state = TURN_OFF;
      }
      break;
    case TURN_OFF:
      break;
  }

}

// EOF
