// src/badguy/ghostflame.cpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux - Ghostflame a flame-like enemy that cannot be killed
// Copyright (C) 2013 LMH <lmh.0013@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#include "badguy/ghostflame.hpp"

#include <math.h>

#include "audio/sound_manager.hpp"
#include "math/random_generator.hpp"
#include "object/sprite_particle.hpp"
#include "sprite/sprite.hpp"
#include "sprite/sprite_manager.hpp"
#include "supertux/object_factory.hpp"
#include "supertux/sector.hpp"
#include "util/reader_mapping.hpp"

Ghostflame::Ghostflame(const ReaderMapping& reader) :
  Flame(reader)
{
  lightsprite->set_color(Color(0.21f, 0.00f, 0.21f));
  sprite = SpriteManager::current()->create("images/creatures/flame/ghostflame.sprite");
}

bool
Ghostflame::is_flammable() const
{
  return false;
}

bool
Ghostflame::is_freezable() const
{
  return false;
}

// EOF
