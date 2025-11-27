// src/badguy/spidermite.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_BADGUY_SPIDERMITE_HPP
#define HEADER_SUPERTUX_BADGUY_SPIDERMITE_HPP

#include "badguy/badguy.hpp"

class SpiderMite : public BadGuy
{
public:
  SpiderMite(const ReaderMapping& reader);
  SpiderMite(const Vector& pos);

  void initialize();
  void active_update(float elapsed_time);
  void collision_solid(const CollisionHit& hit);

  void freeze();
  void unfreeze();
  bool is_freezable() const;
  std::string get_class() const {
    return "spidermite";
  }
  std::string get_display_name() const {
    return _("Spider mite");
  }

protected:
  enum SpiderMiteMode {
    FLY_UP,
    FLY_DOWN
  };

protected:
  bool collision_squished(GameObject& object);

private:
  SpiderMiteMode mode;
  Timer timer;
};

#endif // HEADER_SUPERTUX_BADGUY_SPIDERMITE_HPP

// EOF
