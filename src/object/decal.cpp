// src/object/decal.cpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux - Decal
// Copyright (C) 2008 Christoph Sommer <christoph.sommer@2008.expires.deltadevelopment.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#include "object/decal.hpp"

#include "supertux/object_factory.hpp"
#include "util/reader.hpp"
#include "util/reader_mapping.hpp"

Decal::Decal(const ReaderMapping& reader) :
  MovingSprite(reader, "images/decal/explanations/billboard-fireflower.png", LAYER_OBJECTS, COLGROUP_DISABLED),
  default_action(),
  solid()
{
  layer = reader_get_layer (reader, /* default = */ LAYER_OBJECTS);

  if (!reader.get("solid", solid)) solid = false;
  if(solid)
    set_group(COLGROUP_STATIC);
  if(reader.get("action", default_action))
    set_action(default_action, -1);
}

Decal::~Decal()
{
}

// EOF
