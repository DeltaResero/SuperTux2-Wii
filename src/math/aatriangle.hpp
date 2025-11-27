// src/math/aatriangle.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_MATH_AATRIANGLE_HPP
#define HEADER_SUPERTUX_MATH_AATRIANGLE_HPP

#include "math/rectf.hpp"

/**
 * An axis-aligned triangle (ie. a triangle where 2 sides are parallel to the x-
 * and y-axis.
 */
class AATriangle
{
public:
  /** Directions:
   *
   *    SOUTHEWEST    NORTHEAST   SOUTHEAST    NORTHWEST
   *    *      or      *---*   or      *    or *---*
   *    | \             \  |         / |       |  /
   *    |  \             \ |        /  |       | /
   *    *---*              *       *---*       *
   *
   * Deform flags: (see docs/aatriangletypes.png for details)
   */
  enum Direction {
    SOUTHWEST = 0,
    NORTHEAST,
    SOUTHEAST,
    NORTHWEST,
    DIRECTION_MASK = 0x0003,
    DEFORM_BOTTOM = 0x0010,
    DEFORM_TOP = 0x0020,
    DEFORM_LEFT = 0x0030,
    DEFORM_RIGHT = 0x0040,
    DEFORM_MASK = 0x0070
  };

  static int vertical_flip(int dir);

public:
  AATriangle() :
    bbox(),
    dir(SOUTHWEST)
  {
  }
  AATriangle(const Rectf& newbbox, int newdir) :
    bbox(newbbox),
    dir(newdir)
  {
  }

public:
  Rectf bbox;
  int dir;
};

#endif // HEADER_SUPERTUX_MATH_AATRIANGLE_HPP

// EOF
