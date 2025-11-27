// src/scripting/level_time.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_SCRIPTING_LEVEL_TIME_HPP
#define HEADER_SUPERTUX_SCRIPTING_LEVEL_TIME_HPP

#ifndef SCRIPTING_API
class LevelTime;
#endif

namespace scripting {

class LevelTime
{
public:
#ifndef SCRIPTING_API
  LevelTime(::LevelTime* level_time);
  ~LevelTime();
#endif

  /**
   * Resumes the countdown
   */
  void start();

  /**
   * Pauses the countdown
   */
  void stop();

  /**
   * Returns the number of seconds left on the clock
   */
  float get_time() const;

  /**
   * Changes the number of seconds left on the clock
   */
  void set_time(float time_left);

#ifndef SCRIPTING_API
  ::LevelTime* level_time;

private:
  LevelTime(const LevelTime&);
  LevelTime& operator=(const LevelTime&);
#endif
};

}

#endif // HEADER_SUPERTUX_SCRIPTING_LEVEL_TIME_HPP

// EOF
