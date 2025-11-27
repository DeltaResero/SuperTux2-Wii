// src/object/rainsplash.cpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#include "object/rainsplash.hpp"

RainSplash::RainSplash(const Vector& pos, bool vertical) :
  sprite(),
  position(pos),
  frame(0)
{
  if (vertical) sprite = SpriteManager::current()->create("images/objects/particles/rainsplash-vertical.sprite");
  else sprite = SpriteManager::current()->create("images/objects/particles/rainsplash.sprite");
}

RainSplash::~RainSplash() {
  remove_me();
}

void
RainSplash::hit(Player& )
{
}

void
RainSplash::update(float /*time*/)
{
  frame++;
  if (frame >= 10) remove_me();
}

void
RainSplash::draw(DrawingContext& context)
{
  sprite->draw(context, position, LAYER_OBJECTS);
}

// EOF
