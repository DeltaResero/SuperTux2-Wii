// src/object/rain_particle_system.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_OBJECT_RAIN_PARTICLE_SYSTEM_HPP
#define HEADER_SUPERTUX_OBJECT_RAIN_PARTICLE_SYSTEM_HPP

#include <memory>

#include "object/particlesystem_interactive.hpp"
#include "video/surface_ptr.hpp"

class RainParticleSystem : public ParticleSystem_Interactive
{
public:
  RainParticleSystem();
  RainParticleSystem(const ReaderMapping& reader);
  virtual ~RainParticleSystem();

  void init();
  virtual void update(float elapsed_time);

  std::string type() const
  { return "RainParticleSystem"; }
  std::string get_class() const {
    return "particles-rain";
  }
  std::string get_display_name() const {
    return _("Rain particles");
  }

private:
  class RainParticle : public Particle
  {
  public:
    float speed;

    RainParticle() :
      speed()
    {}
  };

  SurfacePtr rainimages[2];

private:
  RainParticleSystem(const RainParticleSystem&);
  RainParticleSystem& operator=(const RainParticleSystem&);
};

#endif // HEADER_SUPERTUX_OBJECT_RAIN_PARTICLE_SYSTEM_HPP

// EOF
