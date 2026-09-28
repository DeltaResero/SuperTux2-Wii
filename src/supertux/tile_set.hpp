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

#ifndef HEADER_SUPERTUX_SUPERTUX_TILE_SET_HPP
#define HEADER_SUPERTUX_SUPERTUX_TILE_SET_HPP

#include <memory>
#include <stdint.h>
#include <string>

#include "math/fwd.hpp"
#include "video/color.hpp"
#include "video/surface_ptr.hpp"

class Canvas;
class DrawingContext;
class Tile;

class TileSet final
{
public:
  static std::unique_ptr<TileSet> from_file(const std::string& filename);

public:
  TileSet();
  ~TileSet();

  void add_tile(int id, std::unique_ptr<Tile> tile);

  const Tile& get(const uint32_t id) const;
  

  void print_debug_info(const std::string& filename);
  
private:
  std::vector<std::unique_ptr<Tile> > m_tiles;

private:
  TileSet(const TileSet&) = delete;
  TileSet& operator=(const TileSet&) = delete;
};

#endif

/* EOF */
