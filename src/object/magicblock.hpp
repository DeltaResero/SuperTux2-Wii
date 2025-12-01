// src/object/magicblock.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux - MagicBlock
// Copyright (C) 2006 Wolfgang Becker <uafr@gmx.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_OBJECT_MAGICBLOCK_HPP
#define HEADER_SUPERTUX_OBJECT_MAGICBLOCK_HPP

#include "object/moving_sprite.hpp"

class MagicBlock: public MovingSprite
{
public:
  MagicBlock(const ReaderMapping& reader);

  bool collides(GameObject& other, const CollisionHit& hit) const;
  HitResponse collision(GameObject& other, const CollisionHit& hit);
  void update(float elapsed_time);
  void draw(DrawingContext& context);
  std::string get_class() const {
    return "magicblock";
  }
  std::string get_display_name() const {
    return _("Magic block");
  }

private:
  bool is_solid;
  float trigger_red;
  float trigger_green;
  float trigger_blue;
  float solid_time;
  float switch_delay; /**< seconds until switching solidity */
  Rectf solid_box;
  Color color;
  Color light;
  Vector center;
  bool black;
};

#endif // HEADER_SUPERTUX_OBJECT_MAGICBLOCK_HPP

// EOF
