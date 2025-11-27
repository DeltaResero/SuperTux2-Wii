// src/object/ghost_particle_system.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2009 Ingo Ruhnke <grumbel@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_OBJECT_GHOST_PARTICLE_SYSTEM_HPP
#define HEADER_SUPERTUX_OBJECT_GHOST_PARTICLE_SYSTEM_HPP

#include <memory>

#include "object/particlesystem.hpp"
#include "video/surface_ptr.hpp"

class ReaderMapping;

class GhostParticleSystem : public ParticleSystem
{
public:
  GhostParticleSystem();
  GhostParticleSystem(const ReaderMapping& reader);
  virtual ~GhostParticleSystem();

  void init();
  virtual void update(float elapsed_time);

  std::string type() const
  { return "GhostParticleSystem"; }
  std::string get_class() const {
    return "particles-ghosts";
  }
  std::string get_display_name() const {
    return _("Ghost particles");
  }

  virtual const std::string get_icon_path() const {
    return "images/engine/editor/ghostparticles.png";
  }

private:
  class GhostParticle : public Particle
  {
  public:
    float speed;

    GhostParticle() :
      speed()
    {}
  };

  SurfacePtr ghosts[2];

private:
  GhostParticleSystem(const GhostParticleSystem&);
  GhostParticleSystem& operator=(const GhostParticleSystem&);
};

#endif // HEADER_SUPERTUX_OBJECT_GHOST_PARTICLE_SYSTEM_HPP

// EOF
