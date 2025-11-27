// src/gui/menu_color.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2015 Hume2 <teratux.mail@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_GUI_MENU_COLOR_HPP
#define HEADER_SUPERTUX_GUI_MENU_COLOR_HPP

#include "gui/menu.hpp"

class Color;

class ColorMenu : public Menu
{
public:
  ColorMenu(Color* color_);

  void menu_action(MenuItem* item) override;

private:
  Color* color;

  ColorMenu(const ColorMenu&);
  ColorMenu& operator=(const ColorMenu&);
};

#endif // HEADER_SUPERTUX_GUI_MENU_COLOR_HPP

// EOF
