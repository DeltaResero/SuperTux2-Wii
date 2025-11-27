// src/object/particlesystem_interactive.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_OBJECT_PARTICLESYSTEM_INTERACTIVE_HPP
#define HEADER_SUPERTUX_OBJECT_PARTICLESYSTEM_INTERACTIVE_HPP

#include "object/particlesystem.hpp"

class Vector;

/**
 * This is an alternative class for particle systems. It is responsible for storing a
 * set of particles with each having an x- and y-coordinate the number of the
 * layer where it should be drawn and a texture.
 * This version of the particle system class doesn't use virtual screen coordinates,
 * but Interactive ones. Particle systems which need Interactive levels coordinates, such
 * as rain, should be implemented here.
 * Classes that implement a particle system should subclass from this class,
 * initialize particles in the constructor and move them in the simulate
 * function.
 */
class ParticleSystem_Interactive : public ParticleSystem
{
public:
  ParticleSystem_Interactive();
  virtual ~ParticleSystem_Interactive();

  virtual void draw(DrawingContext& context) override;
  virtual std::string get_display_name() const override
  {
    return _("Interactive particle system");
  }

protected:
  int collision(Particle* particle, Vector movement);

};

#endif // HEADER_SUPERTUX_OBJECT_PARTICLESYSTEM_INTERACTIVE_HPP

// EOF
