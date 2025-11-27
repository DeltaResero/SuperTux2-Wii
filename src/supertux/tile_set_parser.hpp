// src/supertux/tile_set_parser.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux - Ingo Ruhnke <grumbel@gmail.com>
// Copyright (C) 2008 Matthias Braun <matze@braunis.de>,
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_SUPERTUX_TILE_SET_PARSER_HPP
#define HEADER_SUPERTUX_SUPERTUX_TILE_SET_PARSER_HPP

#include <string>
#include <stdint.h>
#include <vector>

#include "supertux/tile.hpp"
#include "util/reader_fwd.hpp"

class TileSet;
class Tile;

class TileSetParser
{
private:
  TileSet&    m_tileset;
  std::string m_filename;
  std::string m_tiles_path;

public:
  TileSetParser(TileSet& tileset, const std::string& filename);

  void parse();

private:
  void parse_tile(const ReaderMapping& reader);
  void parse_tiles(const ReaderMapping& reader);
  std::vector<Tile::ImageSpec> parse_imagespecs(const ReaderMapping& cur) const;

private:
  TileSetParser(const TileSetParser&);
  TileSetParser& operator=(const TileSetParser&);
};

#endif // HEADER_SUPERTUX_SUPERTUX_TILE_SET_PARSER_HPP

// EOF
