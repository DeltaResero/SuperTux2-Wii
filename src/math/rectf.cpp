// src/math/rectf.cpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2010 Ingo Ruhnke <grumbel@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#include "math/rectf.hpp"

#include "math/sizef.hpp"

Rectf::Rectf(const Vector& p1_, const Sizef& size) :
  p1(p1_),
  p2(p1_.x + size.width, p1_.y + size.height)
{
}

// EOF
