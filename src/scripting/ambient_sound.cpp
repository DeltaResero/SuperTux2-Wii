// src/scripting/ambient_sound.cpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2015 Ingo Ruhnke <grumbel@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#include "scripting/ambient_sound.hpp"

#include "object/ambient_sound.hpp"

namespace scripting {

AmbientSound::AmbientSound(::AmbientSound* parent) :
  m_parent(parent)
{
}

AmbientSound::~AmbientSound()
{
}

void
AmbientSound::set_pos(float x, float y)
{
  m_parent->set_pos(x, y);
}

float
AmbientSound::get_pos_x() const
{
  return m_parent->get_pos_x();
}

float
AmbientSound::get_pos_y() const
{
  return m_parent->get_pos_y();
}

} // namespace scripting

// EOF
