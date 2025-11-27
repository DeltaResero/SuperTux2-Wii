// src/video/color.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_VIDEO_COLOR_HPP
#define HEADER_SUPERTUX_VIDEO_COLOR_HPP

#include <assert.h>
#include <vector>

#include "util/log.hpp"

class Color
{
public:
  Color();

  Color(float red_, float green_, float blue_, float alpha_ = 1.0);

  Color(const std::vector<float>& vals);

  bool operator==(const Color& other) const;

  float greyscale() const;

  bool operator < (const Color& other) const;

  std::vector<float> toVector();

  float red, green, blue, alpha;

  static const Color BLACK;
  static const Color RED;
  static const Color GREEN;
  static const Color BLUE;
  static const Color CYAN;
  static const Color MAGENTA;
  static const Color YELLOW;
  static const Color WHITE;
};

#endif // HEADER_SUPERTUX_VIDEO_COLOR_HPP

// EOF
