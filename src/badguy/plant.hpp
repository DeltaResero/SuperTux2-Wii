// src/badguy/plant.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_BADGUY_PLANT_HPP
#define HEADER_SUPERTUX_BADGUY_PLANT_HPP

#include "badguy/badguy.hpp"

class Plant : public BadGuy
{
public:
  Plant(const ReaderMapping& reader);

  void initialize();
  void collision_solid(const CollisionHit& hit);
  HitResponse collision_badguy(BadGuy& badguy, const CollisionHit& hit);
  void active_update(float elapsed_time);
  void ignite();
  std::string get_class() const {
    return "plant";
  }
  std::string get_display_name() const {
    return _("Plant");
  }

protected:
  enum PlantState {
    PLANT_SLEEPING,
    PLANT_WAKING,
    PLANT_WALKING
  };

private:
  Timer timer;
  PlantState state;
};

#endif // HEADER_SUPERTUX_BADGUY_PLANT_HPP

// EOF
