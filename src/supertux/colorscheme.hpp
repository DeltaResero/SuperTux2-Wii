// src/supertux/colorscheme.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2014 Ingo Ruhnke <grumbel@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_SUPERTUX_COLORSCHEME_HPP
#define HEADER_SUPERTUX_SUPERTUX_COLORSCHEME_HPP

#include "video/color.hpp"

class ColorScheme
{
public:
  class Menu
  {
  public:
    static Color default_color;
    static Color active_color;
    static Color inactive_color;
    static Color label_color;
    static Color field_color;
  };
};

#endif // HEADER_SUPERTUX_SUPERTUX_COLORSCHEME_HPP

// EOF
