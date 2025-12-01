// src/object/snow_particle_system.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_OBJECT_SNOW_PARTICLE_SYSTEM_HPP
#define HEADER_SUPERTUX_OBJECT_SNOW_PARTICLE_SYSTEM_HPP

#include "object/particlesystem.hpp"
#include "supertux/timer.hpp"

class ReaderMapping;

class SnowParticleSystem : public ParticleSystem
{
public:
  SnowParticleSystem();
  SnowParticleSystem(const ReaderMapping& reader);
  virtual ~SnowParticleSystem();

  void init();
  virtual void update(float elapsed_time);

  std::string type() const
  { return "SnowParticleSystem"; }
  std::string get_class() const {
    return "particles-snow";
  }
  std::string get_display_name() const {
    return _("Snow particles");
  }

private:
  class SnowParticle : public Particle
  {
  public:
    float speed;
    float wobble;
    float anchorx;
    float drift_speed;

    // Turning speed
    float spin_speed;

    // for inertia
    unsigned int flake_size;

    SnowParticle() :
      speed(),
      wobble(),
      anchorx(),
      drift_speed(),
      spin_speed(),
      flake_size()
    {}
  };

  // Wind is simulated in discrete "gusts"

  // Gust state
  enum State {
    ATTACKING,
    DECAYING,
    SUSTAINING,
    RELEASING,
    RESTING,
    MAX_STATE
  };
  State state;


  // Gust state delay timer
  Timer timer;

  // Peak magnitude of gust is gust_onset * randf(5)
  float gust_onset,
  // Current blowing velocity of gust
        gust_current_velocity;

  SurfacePtr snowimages[3];

private:
  SnowParticleSystem(const SnowParticleSystem&);
  SnowParticleSystem& operator=(const SnowParticleSystem&);
};

#endif // HEADER_SUPERTUX_OBJECT_SNOW_PARTICLE_SYSTEM_HPP

// EOF
