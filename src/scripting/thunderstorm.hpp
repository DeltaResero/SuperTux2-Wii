// src/scripting/thunderstorm.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_SCRIPTING_THUNDERSTORM_HPP
#define HEADER_SUPERTUX_SCRIPTING_THUNDERSTORM_HPP

#ifndef SCRIPTING_API
class Thunderstorm;
#endif

namespace scripting {

class Thunderstorm
{
public:
#ifndef SCRIPTING_API
  Thunderstorm(::Thunderstorm* thunderstorm);
  ~Thunderstorm();
#endif

  /**
   * Start playing thunder and lightning at configured interval
   */
  void start();

  /**
   * Stop playing thunder and lightning at configured interval
   */
  void stop();

  /**
   * Play thunder
   */
  void thunder();

  /**
   * Play lightning, i.e. call flash() and electrify()
   */
  void lightning();

  /**
   * Display a nice flash
   */
  void flash();

  /**
   * Electrify water throughout the whole sector for a short time
   */
  void electrify();

#ifndef SCRIPTING_API
  ::Thunderstorm* thunderstorm;

private:
  Thunderstorm(const Thunderstorm&);
  Thunderstorm& operator=(const Thunderstorm&);
#endif
};

}

#endif // HEADER_SUPERTUX_SCRIPTING_THUNDERSTORM_HPP

// EOF
