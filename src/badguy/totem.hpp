// src/badguy/totem.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux - "Totem" Badguy
// Copyright (C) 2006 Christoph Sommer <christoph.sommer@2006.expires.deltadevelopment.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_BADGUY_TOTEM_HPP
#define HEADER_SUPERTUX_BADGUY_TOTEM_HPP

#include "badguy/badguy.hpp"

/**
 * "Totem" Badguy - A variable-height stack of wooden blocks
 */
class Totem : public BadGuy
{
public:
  Totem(const ReaderMapping& reader);
  ~Totem();

  void initialize();
  void active_update(float elapsed_time);
  void collision_solid(const CollisionHit& hit);
  HitResponse collision_badguy(BadGuy& badguy, const CollisionHit& hit);

  virtual bool updatePointers(const GameObject* from_object, GameObject* to_object);
  std::string get_class() const {
    return "totem";
  }
  std::string get_display_name() const {
    return _("Totem");
  }

protected:
  bool collision_squished(GameObject& object);
  void kill_fall();

  void jump_on(Totem* target); /**< jump on target */
  void jump_off(); /**< jump off current base */

  void synchronize_with(Totem* baseTotem); /**< synchronize position and movement with baseTotem */

private:
  Totem* carrying; /**< Totem we are currently carrying (or 0) */
  Totem* carried_by; /**< Totem by which we are currently carried (or 0) */

private:
  Totem(const Totem&);
  Totem& operator=(const Totem&);
};

#endif // HEADER_SUPERTUX_BADGUY_TOTEM_HPP

// EOF
