// src/editor/tile_selection.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2016 Hume2 <teratux.mail@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef  HEADER_SUPERTUX_EDITOR_TILE_SELECTION_HPP
#define  HEADER_SUPERTUX_EDITOR_TILE_SELECTION_HPP

#include <algorithm>
#include <stdint.h>
#include <vector>

class TileSelection
{
  public:
    TileSelection();

    std::vector<uint32_t> tiles;
    int width, height;

    /** Returns the tile id at specific coordinates.
     *  The coordinates might be bigger than the selection size or negative.
     */
    uint32_t pos(int x, int y) const;

    /**
     * Sets the tile selection to a single tile.
     */
    void set_tile(uint32_t tile);

    /**
     * Returns true when has no tiles or is full of zeros
     */
    bool empty() const;
};

#endif // HEADER_SUPERTUX_EDITOR_TILE_SELECTION_HPP

// EOF
