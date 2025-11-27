// src/object/spotlight.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_OBJECT_SPOTLIGHT_HPP
#define HEADER_SUPERTUX_OBJECT_SPOTLIGHT_HPP

#include <memory>

#include "sprite/sprite_ptr.hpp"
#include "supertux/moving_object.hpp"
#include "video/color.hpp"

class ReaderMapping;

class Spotlight : public MovingObject
{
public:
  Spotlight(const ReaderMapping& reader);
  virtual ~Spotlight();

  void update(float elapsed_time);
  void draw(DrawingContext& context);

  HitResponse collision(GameObject& other, const CollisionHit& hit_);

  std::string get_class() const {
    return "spotlight";
  }
  std::string get_display_name() const {
    return _("Spotlight");
  }

  virtual ObjectSettings get_settings();

private:
  float   angle;
  SpritePtr center;
  SpritePtr base;
  SpritePtr lights;
  SpritePtr light;
  SpritePtr lightcone;

  Color   color;
};

#endif // HEADER_SUPERTUX_OBJECT_SPOTLIGHT_HPP

// EOF
