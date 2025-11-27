// src/editor/point_marker.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2016 Hume2 <teratux.mail@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_POINT_MARKER_HPP
#define HEADER_SUPERTUX_POINT_MARKER_HPP

#include "supertux/moving_object.hpp"
#include "video/drawing_context.hpp"

class PointMarker : public MovingObject
{
  public:
    PointMarker(const Vector& pos);
    PointMarker();
    ~PointMarker();

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

    virtual Vector get_point_vector() const;
    virtual Vector get_offset() const;

  private:
    PointMarker(const PointMarker&);
    PointMarker& operator=(const PointMarker&);
};

#endif // HEADER_SUPERTUX_POINT_MARKER_HPP

// EOF
