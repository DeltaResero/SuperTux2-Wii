// src/object/falling_coin.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Ondrej Hosek <ondra.hosek@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_OBJECT_FALLING_COIN_HPP
#define HEADER_SUPERTUX_OBJECT_FALLING_COIN_HPP

#include "sprite/sprite.hpp"
#include "supertux/game_object.hpp"
#include "supertux/physic.hpp"

class FallingCoin : public GameObject
{
public:
  FallingCoin(const Vector& start_position, const int x_vel);
  ~FallingCoin();

  void draw(DrawingContext& context);
  void update(float elapsed_time);

private:
  Physic physic;
  Vector pos;
  SpritePtr sprite;
};

#endif // HEADER_SUPERTUX_OBJECT_FALLING_COIN_HPP

// EOF
