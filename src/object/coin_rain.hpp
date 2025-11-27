// src/object/coin_rain.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2013 LMH <lmh.0013@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_OBJECT_COIN_RAIN_HPP
#define HEADER_SUPERTUX_OBJECT_COIN_RAIN_HPP

#include <memory>

#include "math/vector.hpp"
#include "sprite/sprite_ptr.hpp"
#include "supertux/game_object.hpp"
#include "supertux/timer.hpp"

class CoinRain : public GameObject
{
public:
  CoinRain(const Vector& pos, bool emerge=false);
  virtual void update(float elapsed_time);
  virtual void draw(DrawingContext& context);
  virtual bool do_save() const {
    return false;
  }

private:
  SpritePtr sprite;
  Vector position;
  float emerge_distance;
  Timer timer;
  int counter;
  int drop;
};

#endif // HEADER_SUPERTUX_OBJECT_COIN_RAIN_HPP

// EOF
