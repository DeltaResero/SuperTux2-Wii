// src/object/portable.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_OBJECT_PORTABLE_HPP
#define HEADER_SUPERTUX_OBJECT_PORTABLE_HPP

#include "supertux/direction.hpp"

class MovingObject;

/**
 * An object that inherits from this object is considered "portable" and can
 * be carried around by the player.
 * The object has to additionally set the PORTABLE flag (this allows to
 * make the object only temporarily portable by resetting the flag)
 */
class Portable
{
public:
  virtual ~Portable()
  { }

  /**
   * called each frame when the object has been grabbed.
   */
  virtual void grab(MovingObject& object, const Vector& pos, Direction dir) = 0;

  virtual void ungrab(MovingObject& , Direction )
  {}

  virtual bool is_portable() const
  {
    return true;
  }

  /**
   * Is the object so heavy/bulky/fragile that Tux can't run while
   * carrying it?
   */
  virtual bool is_hampering() const
  {
    return false;
  }
};

#endif // HEADER_SUPERTUX_OBJECT_PORTABLE_HPP

// EOF
