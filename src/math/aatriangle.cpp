// src/math/aatriangle.cpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2009 Matt McCutchen
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#include "aatriangle.hpp"

int AATriangle::vertical_flip(int dir) {
  int direction = dir & AATriangle::DIRECTION_MASK;
  direction = 3 - direction;
  int deform = dir & AATriangle::DEFORM_MASK;
  switch (deform) {
    case AATriangle::DEFORM_BOTTOM:
      deform = AATriangle::DEFORM_TOP;
      break;
    case AATriangle::DEFORM_TOP:
      deform = AATriangle::DEFORM_BOTTOM;
      break;
    default:
      // unchanged
      break;
  }
  return (direction | deform);
}

// EOF
