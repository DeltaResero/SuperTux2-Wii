// src/object/sprite_particle.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
// Copyright (C) 2006 Christoph Sommer <christoph.sommer@2006.expires.deltadevelopment.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_OBJECT_SPRITE_PARTICLE_HPP
#define HEADER_SUPERTUX_OBJECT_SPRITE_PARTICLE_HPP

#include "object/anchor_point.hpp"
#include "sprite/sprite.hpp"
#include "sprite/sprite_manager.hpp"

class Player;

class SpriteParticle : public GameObject
{
public:
  SpriteParticle(const std::string& sprite_name, const std::string& action, Vector position, AnchorPoint anchor, Vector velocity, Vector acceleration, int drawing_layer = LAYER_OBJECTS-1);
  ~SpriteParticle();
protected:
  virtual void hit(Player& player);
  virtual void update(float elapsed_time);
  virtual void draw(DrawingContext& context);
  virtual bool do_save() const {
    return false;
  }

private:
  SpritePtr sprite;
  Vector position;
  Vector velocity;
  Vector acceleration;
  int drawing_layer;
  Color light;
  SpritePtr lightsprite;
  bool glow;

private:
  SpriteParticle(const SpriteParticle&);
  SpriteParticle& operator=(const SpriteParticle&);
};

#endif // HEADER_SUPERTUX_OBJECT_SPRITE_PARTICLE_HPP

// EOF
