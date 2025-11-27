// src/object/bouncy_coin.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_OBJECT_BOUNCY_COIN_HPP
#define HEADER_SUPERTUX_OBJECT_BOUNCY_COIN_HPP

#include <memory>

#include "math/vector.hpp"
#include "sprite/sprite_ptr.hpp"
#include "supertux/game_object.hpp"
#include "supertux/timer.hpp"

class BouncyCoin : public GameObject
{
public:
  BouncyCoin(const Vector& pos, bool emerge = false,
             const std::string& sprite_path = "images/objects/coin/coin.sprite");
  ~BouncyCoin();
  virtual void update(float elapsed_time);
  virtual void draw(DrawingContext& context);
  virtual bool do_save() const {
    return false;
  }

private:
  SpritePtr sprite;
  Vector position;
  Timer timer;
  float emerge_distance;

private:
  BouncyCoin(const BouncyCoin&);
  BouncyCoin& operator=(const BouncyCoin&);
};

#endif // HEADER_SUPERTUX_OBJECT_BOUNCY_COIN_HPP

// EOF
