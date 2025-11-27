// src/object/trampoline.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux - Trampoline
// Copyright (C) 2006 Wolfgang Becker <uafr@gmx.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_OBJECT_TRAMPOLINE_HPP
#define HEADER_SUPERTUX_OBJECT_TRAMPOLINE_HPP

#include "object/rock.hpp"

/**
 * Jumping on a trampoline makes tux jump higher.
 */
class Trampoline : public Rock
{
public:
  Trampoline(const ReaderMapping& reader);
  Trampoline(const Vector& pos, bool port);

  HitResponse collision(GameObject& other, const CollisionHit& hit);
  void update(float elapsed_time);

  void grab(MovingObject&, const Vector& pos, Direction);
  bool is_portable() const;
  std::string get_class() const {
    return "trampoline";
  }
  std::string get_display_name() const {
    return _("Trampoline");
  }

  virtual ObjectSettings get_settings();

private:
  bool portable;

};

#endif // HEADER_SUPERTUX_OBJECT_TRAMPOLINE_HPP

// EOF
