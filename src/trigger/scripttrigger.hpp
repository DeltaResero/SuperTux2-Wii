// src/trigger/scripttrigger.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_TRIGGER_SCRIPTTRIGGER_HPP
#define HEADER_SUPERTUX_TRIGGER_SCRIPTTRIGGER_HPP

#include "trigger/trigger_base.hpp"

class Writer;

class ScriptTrigger : public TriggerBase
{
public:
  ScriptTrigger(const ReaderMapping& reader);
  ScriptTrigger(const Vector& pos, const std::string& script);
  ~ScriptTrigger();
  std::string get_class() const {
    return "scripttrigger";
  }

  void write(Writer& writer);
  void event(Player& player, EventType type);
  void draw(DrawingContext& context);

private:
  EventType triggerevent;
  std::string script;
  bool must_activate;
};

#endif // HEADER_SUPERTUX_TRIGGER_SCRIPTTRIGGER_HPP

// EOF
