// src/scripting/display_effect.cpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2015 Ingo Ruhnke <grumbel@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#include "scripting/display_effect.hpp"

#include "object/display_effect.hpp"

namespace scripting {

DisplayEffect::DisplayEffect(::DisplayEffect* parent) :
  m_parent(parent)
{
}

DisplayEffect::~DisplayEffect()
{
}

void
DisplayEffect::fade_out(float fadetime)
{
  m_parent->fade_out(fadetime);
}

void
DisplayEffect::fade_in(float fadetime)
{
  m_parent->fade_in(fadetime);
}

void
DisplayEffect::set_black(bool enabled)
{
  m_parent->set_black(enabled);
}

bool
DisplayEffect::is_black() const
{
  return m_parent->is_black();
}

void
DisplayEffect::sixteen_to_nine(float fadetime)
{
  m_parent->sixteen_to_nine(fadetime);
}

void
DisplayEffect::four_to_three(float fadetime)
{
  m_parent->four_to_three(fadetime);
}

} // namespace scripting

// EOF
