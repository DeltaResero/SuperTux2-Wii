// src/object/wind.cpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux - Wind
// Copyright (C) 2006 Christoph Sommer <christoph.sommer@2006.expires.deltadevelopment.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#include "object/wind.hpp"

#include "math/random_generator.hpp"
#include "object/particles.hpp"
#include "object/player.hpp"
#include "scripting/squirrel_util.hpp"
#include "supertux/object_factory.hpp"
#include "supertux/sector.hpp"
#include "util/reader_mapping.hpp"
#include "video/drawing_context.hpp"

Wind::Wind(const ReaderMapping& reader) :
  ExposedObject<Wind, scripting::Wind>(this),
  blowing(),
  speed(),
  acceleration(),
  new_size(),
  elapsed_time(0)
{
  float w,h;
  if (!reader.get("name", name)) name = "";
  if (!reader.get("x", bbox.p1.x)) bbox.p1.x = 0;
  if (!reader.get("y", bbox.p1.y)) bbox.p1.y = 0;
  if (!reader.get("width", w)) w = 32;
  if (!reader.get("height", h)) h = 32;
  bbox.set_size(w, h);

  if (!reader.get("blowing", blowing)) blowing = true;

  if (!reader.get("speed-x", speed.x)) speed.x = 0;
  if (!reader.get("speed-y", speed.y)) speed.y = 0;

  if (!reader.get("acceleration", acceleration)) acceleration = 100;

  set_group(COLGROUP_TOUCHABLE);
}

void
Wind::update(float elapsed_time_)
{
  this->elapsed_time = elapsed_time_;

  if (!blowing) return;
  if (bbox.get_width() <= 16 || bbox.get_height() <= 16) return;

  // TODO: nicer, configurable particles for wind?
  if (graphicsRandom.rand(0, 100) < 20) {
    // emit a particle
    Vector ppos = Vector(graphicsRandom.randf(bbox.p1.x+8, bbox.p2.x-8), graphicsRandom.randf(bbox.p1.y+8, bbox.p2.y-8));
    Vector pspeed = Vector(speed.x, speed.y);
    Sector::current()->add_object(std::make_shared<Particles>(ppos, 44, 46, pspeed, Vector(0,0), 1, Color(.4f, .4f, .4f), 3, .1f,
                                                LAYER_BACKGROUNDTILES+1));
  }
}

void
Wind::draw(DrawingContext& context)
{
}

HitResponse
Wind::collision(GameObject& other, const CollisionHit& )
{
  if (!blowing) return ABORT_MOVE;

  auto player = dynamic_cast<Player*> (&other);
  if (player) {
    if (!player->on_ground()) {
      player->add_velocity(speed * acceleration * elapsed_time, speed);
    }
  }

  return ABORT_MOVE;
}

void
Wind::start()
{
  blowing = true;
}

void
Wind::stop()
{
  blowing = false;
}

// EOF
