// src/supertux/menu/contrib_menu.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2009 Ingo Ruhnke <grumbel@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_SUPERTUX_MENU_CONTRIB_MENU_HPP
#define HEADER_SUPERTUX_SUPERTUX_MENU_CONTRIB_MENU_HPP

#include "gui/menu.hpp"

class World;

class ContribMenu : public Menu
{
private:
  std::vector<std::unique_ptr<World> > m_contrib_worlds;

public:
  ContribMenu();
  ~ContribMenu();

  void menu_action(MenuItem* item) override;

private:
  ContribMenu(const ContribMenu&);
  ContribMenu& operator=(const ContribMenu&);
};

#endif // HEADER_SUPERTUX_SUPERTUX_MENU_CONTRIB_MENU_HPP

// EOF
