// src/editor/spawnpoint_marker.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2015 Hume2 <teratux.mail@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_EDITOR_SPAWNPOINT_MARKER_HPP
#define HEADER_SUPERTUX_EDITOR_SPAWNPOINT_MARKER_HPP

#include "supertux/moving_object.hpp"
#include "util/reader_mapping.hpp"
#include "video/surface.hpp"
#include "video/drawing_context.hpp"

class SpawnPoint;

class SpawnPointMarker : public MovingObject
{
  public:
    SpawnPointMarker(const ReaderMapping& lisp);
    SpawnPointMarker(const SpawnPoint* sp);
    ~SpawnPointMarker();

    void update(float elapsed_time) {
      //No updates needed
    }

    void draw(DrawingContext& context);

    void collision_solid(const CollisionHit& hit) {
      //This function wouldn't be called anyway.
    }

    HitResponse collision(GameObject& other, const CollisionHit& hit) {
      return FORCE_MOVE;
    }

    virtual std::string get_class() const {
      return "spawnpoint";
    }
    virtual std::string get_display_name() const {
      return _("Spawn point");
    }

  private:
    SurfacePtr surface;

    void setup();
};

#endif // HEADER_SUPERTUX_EDITOR_SPAWNPOINT_MARKER_HPP

// EOF
