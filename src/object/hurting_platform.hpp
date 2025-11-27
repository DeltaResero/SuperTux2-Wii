// src/object/hurting_platform.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux - Hurting Platform
// Copyright (C) 2006 Christoph Sommer <christoph.sommer@2006.expires.deltadevelopment.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_OBJECT_HURTING_PLATFORM_HPP
#define HEADER_SUPERTUX_OBJECT_HURTING_PLATFORM_HPP

#include "object/platform.hpp"

/**
 * Platform that hurts Tux and Badguys when touched
 */
class HurtingPlatform : public Platform
{
public:
  HurtingPlatform(const ReaderMapping& reader);

  virtual HitResponse collision(GameObject& other, const CollisionHit& hit);
  std::string get_class() const {
    return "hurting_platform";
  }
  std::string get_display_name() const {
    return _("Hurting platform");
  }

private:

};

#endif // HEADER_SUPERTUX_OBJECT_HURTING_PLATFORM_HPP

// EOF
