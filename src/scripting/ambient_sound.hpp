// src/scripting/ambient_sound.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_SCRIPTING_AMBIENT_SOUND_HPP
#define HEADER_SUPERTUX_SCRIPTING_AMBIENT_SOUND_HPP

#ifndef SCRIPTING_API
class AmbientSound;
#endif

namespace scripting {

class AmbientSound
{
#ifndef SCRIPTING_API
private:
  ::AmbientSound* m_parent;

public:
  AmbientSound(::AmbientSound* parent);
   ~AmbientSound();

private:
  AmbientSound(const AmbientSound&) = delete;
  AmbientSound& operator=(const AmbientSound&) = delete;
#endif

public:
  void set_pos(float x, float y);
  float get_pos_x() const;
  float get_pos_y() const;
};

}

#endif // HEADER_SUPERTUX_SCRIPTING_AMBIENT_SOUND_HPP

// EOF
