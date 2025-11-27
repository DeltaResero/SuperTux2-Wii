// src/object/coin_explode.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux - several coins are hurled through the air
// Copyright (C) 2013 LMH <lmh.0013@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_OBJECT_COIN_EXPLODE_HPP
#define HEADER_SUPERTUX_OBJECT_COIN_EXPLODE_HPP

#include <memory>

#include "math/vector.hpp"
#include "supertux/game_object.hpp"

class CoinExplode : public GameObject
{
public:
  CoinExplode(const Vector& pos);
  virtual void update(float elapsed_time);
  virtual void draw(DrawingContext& context);
  virtual bool do_save() const {
    return false;
  }

private:
  Vector position;
};

#endif // HEADER_SUPERTUX_OBJECT_COIN_EXPLODE_HPP

// EOF
