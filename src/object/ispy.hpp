// src/object/ispy.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux - Ispy
// Copyright (C) 2007 Christoph Sommer <christoph.sommer@2007.expires.deltadevelopment.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_OBJECT_ISPY_HPP
#define HEADER_SUPERTUX_OBJECT_ISPY_HPP

#include "object/moving_sprite.hpp"
#include "supertux/direction.hpp"

/**
 * An Ispy: When it spots Tux, a script will run.
 */
class Ispy : public MovingSprite
{
public:
  Ispy(const ReaderMapping& lisp);
  virtual void save(Writer& writer);

  HitResponse collision(GameObject& other, const CollisionHit& hit);

  virtual void update(float elapsed_time);
  std::string get_class() const {
    return "ispy";
  }
  std::string get_display_name() const {
    return _("Ispy");
  }

  virtual ObjectSettings get_settings();
  virtual void after_editor_set();

private:
  enum IspyState {
    ISPYSTATE_IDLE,
    ISPYSTATE_ALERT,
    ISPYSTATE_HIDING,
    ISPYSTATE_SHOWING
  };
  IspyState state; /**< current state */

  std::string script; /**< script to execute when Tux is spotted */
  Direction dir;

};

#endif // HEADER_SUPERTUX_OBJECT_ISPY_HPP

// EOF
