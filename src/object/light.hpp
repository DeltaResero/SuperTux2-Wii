// src/object/light.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_OBJECT_LIGHT_HPP
#define HEADER_SUPERTUX_OBJECT_LIGHT_HPP

#include <memory>

#include "math/vector.hpp"
#include "sprite/sprite_ptr.hpp"
#include "supertux/game_object.hpp"
#include "video/color.hpp"

class Light : public GameObject
{
public:
  Light(const Vector& center, const Color& color = Color(1.0, 1.0, 1.0, 1.0));
  virtual ~Light();
  virtual bool do_save() const {
    return false;
  }

  void update(float elapsed_time);
  void draw(DrawingContext& context);

protected:
  Vector position;
  Color color;
  SpritePtr sprite;
};

#endif // HEADER_SUPERTUX_OBJECT_LIGHT_HPP

// EOF
