// src/object/decal.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux - Decal
// Copyright (C) 2008 Christoph Sommer <christoph.sommer@2008.expires.deltadevelopment.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_OBJECT_DECAL_HPP
#define HEADER_SUPERTUX_OBJECT_DECAL_HPP

#include "object/moving_sprite.hpp"

class ReaderMapping;

/**
 * A decorative image, perhaps part of the terrain
 */
class Decal : public MovingSprite
{
public:
  Decal(const ReaderMapping& reader);
  virtual ~Decal();

  virtual HitResponse collision(GameObject& , const CollisionHit& ) {
    return FORCE_MOVE;
  }
  std::string get_class() const {
    return "decal";
  }
  std::string get_display_name() const {
    return _("Decal");
  }

  virtual ObjectSettings get_settings();

private:
  std::string default_action;
  bool solid;
};

#endif // HEADER_SUPERTUX_OBJECT_DECAL_HPP

// EOF
