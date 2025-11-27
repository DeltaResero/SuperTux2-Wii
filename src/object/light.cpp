// src/object/light.cpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#include "object/light.hpp"
#include "sprite/sprite.hpp"
#include "sprite/sprite_manager.hpp"

Light::Light(const Vector& center, const Color& color_) :
  position(center),
  color(color_),
  sprite()
{
  sprite = SpriteManager::current()->create("images/objects/lightmap_light/lightmap_light.sprite");
}

Light::~Light()
{
}

void
Light::update(float )
{
}

void
Light::draw(DrawingContext& context)
{
  context.push_target();
  context.set_target(DrawingContext::LIGHTMAP);

  sprite->set_color(color);
  sprite->set_blend(Blend(GL_SRC_ALPHA, GL_ONE));
  sprite->draw(context, position, 0);

  context.pop_target();
}

// EOF
