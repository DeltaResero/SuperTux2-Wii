// src/math/vector.cpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
//  SuperTux
//  Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include <math.h>

#include "math/vector.hpp"

Vector Vector::unit() const
{
  const float length = norm();
  /* A vector of no length points nowhere, and dividing by that length would
     hand the caller a NaN that spreads into whatever position it is added to.
     Answer with no direction instead. */
  if (length == 0.0f) return Vector(0.0f, 0.0f);

  return *this / length;
}

float Vector::norm() const
{
  return sqrt(x*x + y*y);
}

/* EOF */
