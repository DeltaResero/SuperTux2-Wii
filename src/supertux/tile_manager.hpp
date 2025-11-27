// src/supertux/tile_manager.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2008 Matthias Braun <matze@braunis.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_SUPERTUX_TILE_MANAGER_HPP
#define HEADER_SUPERTUX_SUPERTUX_TILE_MANAGER_HPP

#include <map>
#include <memory>
#include <string>

#include "util/currenton.hpp"

class TileSet;

class TileManager : public Currenton<TileManager>
{
private:
  typedef std::map<std::string, std::unique_ptr<TileSet> > TileSets;
  TileSets tilesets;

public:
  TileManager();
  ~TileManager();

  TileSet* get_tileset(const std::string &filename);
};

#endif // HEADER_SUPERTUX_SUPERTUX_TILE_MANAGER_HPP

// EOF
