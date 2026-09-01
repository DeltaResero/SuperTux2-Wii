// src/supertux/tile_manager.cpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
//  SuperTux
//  Copyright (C) 2004 Tobias Glaesser <tobi.web@gmx.de>
//  Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "supertux/tile_manager.hpp"

#include <algorithm>

#include <limits>
#include <utility>

#include "supertux/tile_set.hpp"
#include "util/reader_collection.hpp"
#include "util/reader_mapping.hpp"

TileManager::TileManager() :
  tilesets(),
  held()
{
}

TileManager::~TileManager()
{
}

std::shared_ptr<TileSet>
TileManager::get_tileset(const std::string &filename)
{
  TileSets::const_iterator i = tilesets.find(filename);
  if(i != tilesets.end())
  {
    if(auto tileset = i->second.lock())
      return tileset;
  }

  auto tileset = std::make_shared<TileSet>(filename);
  tilesets[filename] = tileset;
  held.push_back(tileset);
  return tileset;
}

void
TileManager::release_unused()
{
  /* Let go first, so that a tileset whose only remaining claim was this list
     goes now and hands its pictures back. */
  held.clear();

  std::erase_if(tilesets, [](const TileSets::value_type& entry) {
    return entry.second.expired();
  });

  /* Then take hold again of whatever is still being drawn from. */
  for(const auto& entry : tilesets) {
    if(auto tileset = entry.second.lock())
      held.push_back(std::move(tileset));
  }
}

/* EOF */
