// src/object/specialriser.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_OBJECT_SPECIALRISER_HPP
#define HEADER_SUPERTUX_OBJECT_SPECIALRISER_HPP

#include "supertux/moving_object.hpp"

/**
 * special object that contains another object and slowly rises it out of a
 * bonus block.
 */
class SpecialRiser : public GameObject
{
public:
  SpecialRiser(const Vector& pos, std::shared_ptr<MovingObject> child);
  ~SpecialRiser();
  virtual bool do_save() const {
    return false;
  }

  virtual void update(float elapsed_time);
  virtual void draw(DrawingContext& context);

private:
  float offset;
  std::shared_ptr<MovingObject> child;

private:
  SpecialRiser(const SpecialRiser&);
  SpecialRiser& operator=(const SpecialRiser&);
};

#endif // HEADER_SUPERTUX_OBJECT_SPECIALRISER_HPP

// EOF
