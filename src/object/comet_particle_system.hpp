// src/object/comet_particle_system.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2009 Ingo Ruhnke <grumbel@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_OBJECT_COMET_PARTICLE_SYSTEM_HPP
#define HEADER_SUPERTUX_OBJECT_COMET_PARTICLE_SYSTEM_HPP

#include <memory>

#include "object/particlesystem_interactive.hpp"
#include "video/surface_ptr.hpp"

class Writer;

class CometParticleSystem : public ParticleSystem_Interactive
{
public:
  CometParticleSystem();
  CometParticleSystem(const ReaderMapping& reader);
  virtual ~CometParticleSystem();

  void write(Writer& writer);

  void init();
  virtual void update(float elapsed_time);

  std::string type() const
  { return "CometParticleSystem"; }
  std::string get_class() const {
    return "particles-comets";
  }
  std::string get_display_name() const {
    return _("Comet particles");
  }

private:
  class CometParticle : public Particle
  {
  public:
    float speed;

    CometParticle() :
      speed()
    {}
  };

  SurfacePtr cometimages[2];

private:
  CometParticleSystem(const CometParticleSystem&);
  CometParticleSystem& operator=(const CometParticleSystem&);
};

#endif // HEADER_SUPERTUX_OBJECT_COMET_PARTICLE_SYSTEM_HPP

// EOF
