// src/math/size.cpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2009 Ingo Ruhnke <grumbel@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#include "math/size.hpp"

#include <ostream>

#include "math/sizef.hpp"

Size::Size(const Sizef& rhs) :
  width(static_cast<int>(rhs.width)),
  height(static_cast<int>(rhs.height))
{
}

std::ostream& operator<<(std::ostream& s, const Size& size)
{
  return s << "Size(" << size.width << ", " << size.height << ")";
}

// EOF
