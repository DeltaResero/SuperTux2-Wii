// src/object/rainsplash.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_OBJECT_RAINSPLASH_HPP
#define HEADER_SUPERTUX_OBJECT_RAINSPLASH_HPP

#include "sprite/sprite.hpp"
#include "sprite/sprite_manager.hpp"
#include "supertux/game_object.hpp"

class Player;

class RainSplash : public GameObject
{
public:
  RainSplash(const Vector& pos, bool vertical);
  ~RainSplash();
  virtual bool do_save() const {
    return false;
  }

protected:
  virtual void hit(Player& );
  virtual void update(float time);
  virtual void draw(DrawingContext& context);

private:
  SpritePtr sprite;
  Vector position;
  int frame;
};

#endif // HEADER_SUPERTUX_OBJECT_RAINSPLASH_HPP

// EOF
