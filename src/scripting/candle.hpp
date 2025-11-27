// src/scripting/candle.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_SCRIPTING_CANDLE_HPP
#define HEADER_SUPERTUX_SCRIPTING_CANDLE_HPP

#ifndef SCRIPTING_API
class Candle;
#endif

namespace scripting {

class Candle
{
public:
#ifndef SCRIPTING_API
  Candle(::Candle* candle);
  ~Candle();
#endif

  bool get_burning() const; /**< returns true if candle is lighted */
  void set_burning(bool burning); /**< true: light candle, false: extinguish candle */

#ifndef SCRIPTING_API
  ::Candle* candle;

private:
  Candle(const Candle&);
  Candle& operator=(const Candle&);
#endif
};

}

#endif // HEADER_SUPERTUX_SCRIPTING_CANDLE_HPP

// EOF
