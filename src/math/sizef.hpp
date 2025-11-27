// src/math/sizef.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2009 Ingo Ruhnke <grumbel@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_MATH_SIZEF_HPP
#define HEADER_SUPERTUX_MATH_SIZEF_HPP

#include <iosfwd>

#include "math/vector.hpp"

class Size;

class Sizef
{
public:
  Sizef() :
    width(0.0f),
    height(0.0f)
  {}

  explicit Sizef(const Vector& v) :
    width(v.x),
    height(v.y)
  {}

  Sizef(float width_, float height_) :
    width(width_),
    height(height_)
  {}

  Sizef(const Sizef& rhs) :
    width(rhs.width),
    height(rhs.height)
  {}

  Sizef(const Size& rhs);

  Sizef& operator*=(float factor)
  {
    width  *= factor;
    height *= factor;
    return *this;
  }

  Sizef& operator/=(float divisor)
  {
    width  /= divisor;
    height /= divisor;
    return *this;
  }

  Sizef& operator+=(const Sizef& rhs)
  {
    width  += rhs.width;
    height += rhs.height;
    return *this;
  }

  Sizef& operator-=(const Sizef& rhs)
  {
    width  -= rhs.width;
    height -= rhs.height;
    return *this;
  }

  Vector as_vector() const
  {
    return Vector(width, height);
  }

public:
  float width;
  float height;
};

inline Sizef operator*(const Sizef& lhs, float factor)
{
  return Sizef(lhs.width  * factor,
               lhs.height * factor);
}

inline Sizef operator*(float factor, const Sizef& rhs)
{
  return Sizef(rhs.width  * factor,
               rhs.height * factor);
}

inline Sizef operator/(const Sizef& lhs, float divisor)
{
  return Sizef(lhs.width  / divisor,
               lhs.height / divisor);
}

inline Sizef operator+(const Sizef& lhs, const Sizef& rhs)
{
  return Sizef(lhs.width  + rhs.width,
               lhs.height + rhs.height);
}

inline Sizef operator-(const Sizef& lhs, const Sizef& rhs)
{
  return Sizef(lhs.width  - rhs.width,
               lhs.height - rhs.height);
}

inline bool operator==(const Sizef& lhs, const Sizef& rhs)
{
  return (lhs.width == rhs.width) && (rhs.height == rhs.height);
}

inline bool operator!=(const Sizef& lhs, const Sizef& rhs)
{
  return (lhs.width != rhs.width) || (lhs.height != rhs.height);
}

std::ostream& operator<<(std::ostream& s, const Sizef& size);

#endif // HEADER_SUPERTUX_MATH_SIZEF_HPP

// EOF
