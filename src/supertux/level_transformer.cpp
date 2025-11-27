// src/supertux/level_transformer.cpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#include "supertux/level.hpp"
#include "supertux/level_transformer.hpp"

LevelTransformer::~LevelTransformer()
{
}

void
LevelTransformer::transform(Level* level)
{
  for(size_t i = 0; i < level->get_sector_count(); ++i) {
    transform_sector(level->get_sector(i));
  }
}

// EOF
