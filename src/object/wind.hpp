// src/object/wind.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux - Wind
// Copyright (C) 2006 Christoph Sommer <christoph.sommer@2006.expires.deltadevelopment.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_OBJECT_WIND_HPP
#define HEADER_SUPERTUX_OBJECT_WIND_HPP

#include "scripting/exposed_object.hpp"
#include "scripting/wind.hpp"
#include "supertux/moving_object.hpp"

class ReaderMapping;

/**
 * Defines an area that will gently push Players in one direction
 */
class Wind : public MovingObject,
             public ExposedObject<Wind, scripting::Wind>
{
public:
  Wind(const ReaderMapping& reader);

  void update(float elapsed_time);
  void draw(DrawingContext& context);
  HitResponse collision(GameObject& other, const CollisionHit& hit);

  /**
   * @name Scriptable Methods
   * @{
   */

  /**
   * start blowing
   */
  void start();

  /**
   * stop blowing
   */
  void stop();

  /**
   * @}
   */

  std::string get_class() const {
    return "wind";
  }
  std::string get_display_name() const {
    return _("Wind");
  }

  virtual ObjectSettings get_settings();

private:
  bool blowing; /**< true if wind is currently switched on */
  Vector speed;
  float acceleration;
  Vector new_size;

  float elapsed_time; /**< stores last elapsed_time gotten at update() */
};

#endif // HEADER_SUPERTUX_OBJECT_WIND_HPP

// EOF
