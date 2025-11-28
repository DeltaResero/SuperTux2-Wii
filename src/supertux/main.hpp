// src/supertux/main.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_SUPERTUX_MAIN_HPP
#define HEADER_SUPERTUX_SUPERTUX_MAIN_HPP

class Main
{
private:
  void init_video();

  void launch_game();

public:
  /** We call it run() instead of main() as main collides with
      #define main SDL_main from SDL.h */
  int run(int argc, char** argv);
};

#endif // HEADER_SUPERTUX_SUPERTUX_MAIN_HPP

// EOF
