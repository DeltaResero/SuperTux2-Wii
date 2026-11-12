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

class TileManager final : public Currenton<TileManager>
{
private:
  /** Tilesets already read, shared by the tilemaps drawing from them and not owned here */
  std::map<std::string, std::weak_ptr<TileSet> > m_tilesets;

  /** Keeps every tileset read until a screen closes, so one lasts between the tilemaps that want it */
  std::vector<std::shared_ptr<TileSet> > m_held;

public:
  TileManager();

  std::shared_ptr<TileSet> get_tileset(const std::string &filename);

  /** Lets go of every tileset nothing draws from any more */
  void release_unused();
};

#endif

/* EOF */
