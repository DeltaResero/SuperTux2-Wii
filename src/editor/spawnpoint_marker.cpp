// src/editor/spawnpoint_marker.cpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2015 Hume2 <teratux.mail@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#include "editor/spawnpoint_marker.hpp"

#include "supertux/globals.hpp"
#include "supertux/resources.hpp"
#include "supertux/spawn_point.hpp"
#include "util/gettext.hpp"
#include "video/renderer.hpp"
#include "video/video_system.hpp"

SpawnPointMarker::SpawnPointMarker (const ReaderMapping& lisp) :
  surface(Surface::create("images/engine/editor/spawnpoint.png"))
{
  if ( !lisp.get("name", name)) name = "";
  if ( !lisp.get("x", bbox.p1.x)) bbox.p1.x = 0;
  if ( !lisp.get("y", bbox.p1.y)) bbox.p1.y = 0;

  setup();
}

SpawnPointMarker::SpawnPointMarker (const SpawnPoint* sp) :
  surface(Surface::create("images/engine/editor/spawnpoint.png"))
{
  name = sp->name;
  bbox.p1 = sp->pos;
  setup();
}

SpawnPointMarker::~SpawnPointMarker() {
}

void SpawnPointMarker::setup() {
  bbox.set_size(32, 32);
}

void SpawnPointMarker::draw(DrawingContext& context) {
  context.draw_surface(surface, bbox.p1, LAYER_FOREGROUND1);
}

/*ObjectSettings
SpawnPointMarker::get_settings() {
  ObjectSettings result(_("Spawn Point"));
  result.options.push_back( ObjectOption(MN_TEXTFIELD, _("Name"), &name));
  return result;
}*/

// EOF
