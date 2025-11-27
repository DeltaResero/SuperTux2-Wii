// src/object/specialriser.cpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#include "video/drawing_context.hpp"
#include "object/specialriser.hpp"
#include "supertux/sector.hpp"

SpecialRiser::SpecialRiser(const Vector& pos, std::shared_ptr<MovingObject> _child) :
  offset(),
  child(_child)
{
  _child->set_pos(pos - Vector(0, 32));
  offset = 0;
}

SpecialRiser::~SpecialRiser()
{
}

void
SpecialRiser::update(float elapsed_time)
{
  offset += 50 * elapsed_time;
  if(offset > 32) {
    Sector::current()->add_object(child);
    remove_me();
  }
}

void
SpecialRiser::draw(DrawingContext& context)
{
  context.push_transform();
  context.set_translation(
    context.get_translation() + Vector(0, -32 + offset));
  child->draw(context);
  context.pop_transform();
}

// EOF
