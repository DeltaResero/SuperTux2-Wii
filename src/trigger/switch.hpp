// src/trigger/switch.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux - Switch Trigger
// Copyright (C) 2006 Christoph Sommer <christoph.sommer@2006.expires.deltadevelopment.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_TRIGGER_SWITCH_HPP
#define HEADER_SUPERTUX_TRIGGER_SWITCH_HPP

#include <string>

#include "trigger/trigger_base.hpp"
#include "util/reader_mapping.hpp"

class Switch : public TriggerBase
{
public:
  Switch(const ReaderMapping& reader);
  virtual ~Switch();
  std::string get_class() const {
    return "switch";
  }

  ObjectSettings get_settings();
  void after_editor_set();

  virtual void update(float elapsed_time);
  virtual void draw(DrawingContext& context);
  virtual void event(Player& player, EventType type);

private:
  enum SwitchState {
    OFF,
    TURN_ON,
    ON,
    TURN_OFF
  };

private:
  std::string sprite_name;
  SpritePtr sprite;
  std::string script;
  std::string off_script;
  SwitchState state;
  bool bistable;
};

#endif // HEADER_SUPERTUX_TRIGGER_SWITCH_HPP

// EOF
