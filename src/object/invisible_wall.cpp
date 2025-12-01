// src/object/invisible_wall.cpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#include "object/invisible_wall.hpp"

#include "util/gettext.hpp"
#include "util/reader_mapping.hpp"
#include "video/drawing_context.hpp"

InvisibleWall::InvisibleWall(const ReaderMapping& lisp):
  width(),
  height()
{
  if (!lisp.get("name" , name)) name = "";
  if (!lisp.get("x", bbox.p1.x)) bbox.p1.x = 0;
  if (!lisp.get("y", bbox.p1.y)) bbox.p1.y = 0;
  if (!lisp.get("width", width)) width = 32;
  if (!lisp.get("height", height)) height = 32;

  bbox.set_size(width, height);

  group = COLGROUP_STATIC;
}

HitResponse
InvisibleWall::collision(GameObject& , const CollisionHit& )
{
  return FORCE_MOVE;
}

void
InvisibleWall::draw(DrawingContext& context)
{
}

void
InvisibleWall::update(float )
{
}

// EOF
