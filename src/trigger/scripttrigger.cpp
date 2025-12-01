// src/trigger/scripttrigger.cpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#include <memory>
#include <sstream>
#include <stdexcept>

#include "supertux/object_factory.hpp"
#include "supertux/sector.hpp"
#include "trigger/scripttrigger.hpp"
#include "util/gettext.hpp"
#include "util/log.hpp"
#include "util/reader_mapping.hpp"
#include "video/drawing_context.hpp"

ScriptTrigger::ScriptTrigger(const ReaderMapping& reader) :
  triggerevent(),
  script(),
  new_size(),
  must_activate(false)
{
  reader.get("x", bbox.p1.x);
  reader.get("y", bbox.p1.y);
  float w = 32, h = 32;
  reader.get("width", w);
  reader.get("height", h);
  bbox.set_size(w, h);
  reader.get("script", script);
  reader.get("button", must_activate);
  if(script.empty()) {
    log_warning << "No script set in script trigger" << std::endl;
  }

  if (must_activate)
    triggerevent = EVENT_ACTIVATE;
  else
    triggerevent = EVENT_TOUCH;
}

ScriptTrigger::ScriptTrigger(const Vector& pos, const std::string& script_) :
  triggerevent(),
  script(),
  new_size(),
  must_activate()
{
  bbox.set_pos(pos);
  bbox.set_size(32, 32);
  this->script = script_;
  triggerevent = EVENT_TOUCH;
}

ScriptTrigger::~ScriptTrigger()
{
}

void
ScriptTrigger::event(Player& , EventType type)
{
  if(type != triggerevent)
    return;

  std::istringstream stream(script);
  Sector::current()->run_script(stream, "ScriptTrigger");
}

void
ScriptTrigger::draw(DrawingContext& context)
{
}

// EOF
