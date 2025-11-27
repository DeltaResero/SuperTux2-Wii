// src/scripting/sector.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux - Sector scripting
// Copyright (C) 2006 Wolfgang Becker <uafr@gmx.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_SCRIPTING_SECTOR_HPP
#define HEADER_SUPERTUX_SCRIPTING_SECTOR_HPP

#ifndef SCRIPTING_API
#include <string>
class Sector;
#endif

namespace scripting {

class Sector
{
#ifndef SCRIPTING_API
private:
  ::Sector* m_parent;

public:
  Sector(::Sector* parent);
  ~Sector();

private:
  Sector(const Sector&) = delete;
  Sector& operator=(const Sector&) = delete;
#endif

public:
  void set_ambient_light(float red, float green, float blue);
  float get_ambient_red() const;
  float get_ambient_green() const;
  float get_ambient_blue() const;

  void set_gravity(float gravity);
  void set_music(const std::string& music);
};

}

#endif // HEADER_SUPERTUX_SCRIPTING_SECTOR_HPP

// EOF
