// src/object/smoke_cloud.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_OBJECT_SMOKE_CLOUD_HPP
#define HEADER_SUPERTUX_OBJECT_SMOKE_CLOUD_HPP

#include "math/vector.hpp"
#include "sprite/sprite_ptr.hpp"
#include "supertux/game_object.hpp"
#include "supertux/timer.hpp"

class SmokeCloud : public GameObject
{
public:
  SmokeCloud(const Vector& pos);
  ~SmokeCloud();

  virtual void update(float elapsed_time);
  virtual void draw(DrawingContext& context);
  virtual bool do_save() const {
    return false;
  }

private:
  SpritePtr sprite;
  Timer timer;
  Vector position;

private:
  SmokeCloud(const SmokeCloud&);
  SmokeCloud& operator=(const SmokeCloud&);
};

#endif // HEADER_SUPERTUX_OBJECT_SMOKE_CLOUD_HPP

// EOF
