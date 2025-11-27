// src/scripting/platform.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_SCRIPTING_PLATFORM_HPP
#define HEADER_SUPERTUX_SCRIPTING_PLATFORM_HPP

#ifndef SCRIPTING_API
class Platform;
#endif

namespace scripting {

class Platform
{
public:
#ifndef SCRIPTING_API
  Platform(::Platform* platform);
  ~Platform();
#endif

  /** Move platform until at given node, then stop */
  void goto_node(int node_no);

  /** Start moving platform */
  void start_moving();

  /** Stop platform at next node */
  void stop_moving();

#ifndef SCRIPTING_API
  ::Platform* platform;

private:
  Platform(const Platform&);
  Platform& operator=(const Platform&);
#endif
};

}

#endif // HEADER_SUPERTUX_SCRIPTING_PLATFORM_HPP

// EOF
