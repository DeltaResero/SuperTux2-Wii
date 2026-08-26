// src/supertux/tile_manager.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
//  SuperTux
//  Copyright (C) 2008 Matthias Braun <matze@braunis.de>
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

#ifndef HEADER_SUPERTUX_SUPERTUX_TILE_MANAGER_HPP
#define HEADER_SUPERTUX_SUPERTUX_TILE_MANAGER_HPP

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "util/currenton.hpp"

class TileSet;

class TileManager : public Currenton<TileManager>
{
private:
  /** Where to find a tileset that has already been read. The manager does
      not own what is listed here; the tilemaps drawing from it do. */
  typedef std::map<std::string, std::weak_ptr<TileSet> > TileSets;
  TileSets tilesets;

  /** The manager's own hold, so a tileset survives between the tilemaps that
      want it. Let go of when a screen closes. */
  std::vector<std::shared_ptr<TileSet> > held;

public:
  TileManager();
  ~TileManager();

  std::shared_ptr<TileSet> get_tileset(const std::string &filename);

  /** Let go of every tileset nothing is drawing from any more. Meant for the
      moment a screen has closed and taken its tilemaps with it. */
  void release_unused();
};

#endif

/* EOF */
