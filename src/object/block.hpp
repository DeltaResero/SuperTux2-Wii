// src/object/block.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_OBJECT_BLOCK_HPP
#define HEADER_SUPERTUX_OBJECT_BLOCK_HPP

#include <memory>

#include "sprite/sprite_ptr.hpp"
#include "supertux/moving_object.hpp"

class Player;
class ReaderMapping;

class Block : public MovingObject
{
public:
  Block(SpritePtr sprite);
  Block(const ReaderMapping& lisp, const std::string& sprite_file);
  ~Block();

  virtual HitResponse collision(GameObject& other, const CollisionHit& hit) override;
  virtual void update(float elapsed_time) override;
  virtual void draw(DrawingContext& context) override;

  virtual ObjectSettings get_settings() override;
  virtual void after_editor_set() override;

protected:
  friend class FlipLevelTransformer;

  virtual void hit(Player& player) = 0;
  void start_bounce(GameObject* hitter);
  void start_break(GameObject* hitter);
  void break_me();

  SpritePtr sprite;
  std::string sprite_name;
  bool bouncing;
  bool breaking;
  float bounce_dir;
  float bounce_offset;
  float original_y;

private:
  Block(const Block&);
  Block& operator=(const Block&);
};

#endif // HEADER_SUPERTUX_OBJECT_BLOCK_HPP

// EOF
