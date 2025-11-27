// src/math/sizef.cpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2009 Ingo Ruhnke <grumbel@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#include "math/sizef.hpp"

#include <ostream>

#include "math/size.hpp"

Sizef::Sizef(const Size& rhs) :
  width(static_cast<float>(rhs.width)),
  height(static_cast<float>(rhs.height))
{
}

std::ostream& operator<<(std::ostream& s, const Sizef& size)
{
  return s << "Size(" << size.width << ", " << size.height << ")";
}

// EOF
