// src/trigger/door.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_TRIGGER_DOOR_HPP
#define HEADER_SUPERTUX_TRIGGER_DOOR_HPP

#include "supertux/timer.hpp"
#include "trigger/trigger_base.hpp"

class Player;
class ReaderMapping;

class Door : public TriggerBase
{
public:
  Door(const ReaderMapping& reader);
  Door(int x, int y, const std::string& sector, const std::string& spawnpoint);
  virtual ~Door();
  std::string get_class() const {
    return "door";
  }

  virtual ObjectSettings get_settings();

  virtual void update(float elapsed_time);
  virtual void draw(DrawingContext& context);
  virtual void event(Player& player, EventType type);
  virtual HitResponse collision(GameObject& other, const CollisionHit& hit);

private:
  enum DoorState {
    CLOSED,
    OPENING,
    OPEN,
    CLOSING
  };

private:
  DoorState state; /**< current state of the door */
  std::string target_sector; /**< target sector to teleport to */
  std::string target_spawnpoint; /**< target spawnpoint to teleport to */
  std::string script;
  SpritePtr sprite; /**< "door" sprite to render */
  Timer stay_open_timer; /**< time until door will close again */
};

#endif // HEADER_SUPERTUX_TRIGGER_DOOR_HPP

// EOF
