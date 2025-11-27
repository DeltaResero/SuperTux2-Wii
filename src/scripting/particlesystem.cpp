// src/scripting/particlesystem.cpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2016 Tobias Markus <tobbi.bugs@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#include "scripting/particlesystem.hpp"

#include "object/particlesystem.hpp"

namespace scripting {

ParticleSystem::ParticleSystem(::ParticleSystem* parent) :
  particlesystem(parent)
{
}

ParticleSystem::~ParticleSystem()
{
}

void ParticleSystem::set_enabled(bool enable)
{
  particlesystem->set_enabled(enable);
}

bool ParticleSystem::get_enabled() const
{
  return particlesystem->get_enabled();
}

}

// EOF
