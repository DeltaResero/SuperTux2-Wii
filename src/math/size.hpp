// src/math/size.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2009 Ingo Ruhnke <grumbel@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_MATH_SIZE_HPP
#define HEADER_SUPERTUX_MATH_SIZE_HPP

#include <iosfwd>

class Sizef;

class Size
{
public:
  Size() :
    width(0),
    height(0)
  {}

  Size(int width_, int height_) :
    width(width_),
    height(height_)
  {}

  Size(const Size& rhs) :
    width(rhs.width),
    height(rhs.height)
  {}

  explicit Size(const Sizef& rhs);

  Size& operator*=(int factor)
  {
    width  *= factor;
    height *= factor;
    return *this;
  }

  Size& operator/=(int divisor)
  {
    width  /= divisor;
    height /= divisor;
    return *this;
  }

  Size& operator+=(const Size& rhs)
  {
    width  += rhs.width;
    height += rhs.height;
    return *this;
  }

  Size& operator-=(const Size& rhs)
  {
    width  -= rhs.width;
    height -= rhs.height;
    return *this;
  }

public:
  int width;
  int height;
};

inline Size operator*(const Size& lhs, int factor)
{
  return Size(lhs.width  * factor,
              lhs.height * factor);
}

inline Size operator*(int factor, const Size& rhs)
{
  return Size(rhs.width  * factor,
              rhs.height * factor);
}

inline Size operator/(const Size& lhs, int divisor)
{
  return Size(lhs.width  / divisor,
              lhs.height / divisor);
}

inline Size operator+(const Size& lhs, const Size& rhs)
{
  return Size(lhs.width  + rhs.width,
              lhs.height + rhs.height);
}

inline Size operator-(const Size& lhs, const Size& rhs)
{
  return Size(lhs.width  - rhs.width,
              lhs.height - rhs.height);
}

inline bool operator==(const Size& lhs, const Size& rhs)
{
  return (lhs.width == rhs.width) && (lhs.height == rhs.height);
}

inline bool operator!=(const Size& lhs, const Size& rhs)
{
  return (lhs.width != rhs.width) || (lhs.height != rhs.height);
}

std::ostream& operator<<(std::ostream& s, const Size& size);

#endif // HEADER_SUPERTUX_MATH_SIZE_HPP

// EOF
