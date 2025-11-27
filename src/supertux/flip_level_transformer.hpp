// src/supertux/flip_level_transformer.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_SUPERTUX_FLIP_LEVEL_TRANSFORMER_HPP
#define HEADER_SUPERTUX_SUPERTUX_FLIP_LEVEL_TRANSFORMER_HPP

#include "supertux/level_transformer.hpp"
#include "video/drawing_context.hpp"

class TileMap;
class BadGuy;
class SpawnPoint;
class MovingObject;
class Flower;
class Platform;
class Block;
class Path;

/** Vertically or horizontally flip a level */
class FlipLevelTransformer : public LevelTransformer
{
public:
  virtual void transform_sector(Sector* sector);

private:
  DrawingEffect transform_drawing_effect(DrawingEffect effect);
  void transform_path(float height, float obj_height, Path& path);
  void transform_tilemap(float height, TileMap& tilemap);
  void transform_moving_object(float height, MovingObject& object);
  void transform_badguy(float height, BadGuy& badguy);
  void transform_spawnpoint(float height, SpawnPoint& spawnpoint);
  void transform_flower(Flower& flower);
  void transform_platform(float height, Platform& platform);
  void transform_block(float height, Block& block);
};

#endif // HEADER_SUPERTUX_SUPERTUX_FLIP_LEVEL_TRANSFORMER_HPP

// EOF
