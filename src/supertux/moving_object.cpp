// src/supertux/moving_object.cpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#include "supertux/moving_object.hpp"

#include "supertux/sector.hpp"

MovingObject::MovingObject() :
  bbox(),
  movement(),
  group(COLGROUP_MOVING),
  dest()
{
}

MovingObject::~MovingObject()
{
}

void
MovingObject::save(Writer& writer) {
  GameObject::save(writer);
  writer.write("x", bbox.p1.x);
  writer.write("y", bbox.p1.y);
}

// EOF
