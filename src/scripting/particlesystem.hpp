// src/scripting/particlesystem.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux - Sector scripting
// Copyright (C) 2016 Tobias Markus <tobbi.bugs@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_SCRIPTING_PARTICLESYSTEM_HPP
#define HEADER_SUPERTUX_SCRIPTING_PARTICLESYSTEM_HPP

#ifndef SCRIPTING_API
class ParticleSystem;
#endif

namespace scripting {

class ParticleSystem
{
public:
#ifndef SCRIPTING_API
  ParticleSystem(::ParticleSystem* parent);
  ~ParticleSystem();
#endif

  void set_enabled(bool enable);
  bool get_enabled() const;

#ifndef SCRIPTING_API
  ::ParticleSystem* particlesystem;

private:
  ParticleSystem(const ParticleSystem&) = delete;
  ParticleSystem& operator=(const ParticleSystem&) = delete;
#endif
};

}

#endif // HEADER_SUPERTUX_SCRIPTING_PARTICLESYSTEM_HPP

// EOF
