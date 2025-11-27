// src/object/brick.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2009 Ingo Ruhnke <grumbel@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_OBJECT_BRICK_HPP
#define HEADER_SUPERTUX_OBJECT_BRICK_HPP

#include "object/block.hpp"

class Brick : public Block
{
public:
  Brick(const Vector& pos, int data, const std::string& spriteName);
  Brick(const ReaderMapping& lisp);

  void try_break(Player* player);
  HitResponse collision(GameObject& other, const CollisionHit& hit);
  virtual ObjectSettings get_settings();
  std::string get_class() const {
    return "brick";
  }
  std::string get_display_name() const {
    return _("Brick");
  }

protected:
  virtual void hit(Player& player);

private:
  bool breakable;
  int coin_counter;
};

#endif // HEADER_SUPERTUX_OBJECT_BRICK_HPP

// EOF
