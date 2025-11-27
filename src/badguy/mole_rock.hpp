// src/badguy/mole_rock.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux - Rock thrown by "Mole" Badguy
// Copyright (C) 2006 Christoph Sommer <christoph.sommer@2006.expires.deltadevelopment.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_BADGUY_MOLE_ROCK_HPP
#define HEADER_SUPERTUX_BADGUY_MOLE_ROCK_HPP

#include "badguy/badguy.hpp"

/**
 * Badguy "MoleRock" - Rock thrown by "Mole" Badguy
 */
class MoleRock : public BadGuy
{
public:
  MoleRock(const ReaderMapping& reader);
  MoleRock(const Vector& pos, const Vector& velocity, const BadGuy* parent);
  ~MoleRock();

  void initialize();
  void deactivate();

  void active_update(float elapsed_time);

  void collision_solid(const CollisionHit& hit);
  HitResponse collision_badguy(BadGuy& badguy, const CollisionHit& hit);
  HitResponse collision_player(Player& player, const CollisionHit& hit);

  virtual bool updatePointers(const GameObject* from_object, GameObject* to_object);

  bool is_flammable() const;
  std::string get_class() const {
    return "mole_rock";
  }
  std::string get_display_name() const {
    return _("Mole's rock");
  }

protected:
  const BadGuy* parent; /**< collisions with this BadGuy will be ignored */
  const Vector initial_velocity; /**< velocity at time of creation */

private:
  MoleRock(const MoleRock&);
  MoleRock& operator=(const MoleRock&);
};

#endif // HEADER_SUPERTUX_BADGUY_MOLE_ROCK_HPP

// EOF
