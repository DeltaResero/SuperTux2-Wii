// src/supertux/menu/world_set_menu.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2015 Matthew <thebatmankiller3@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef WORLD_SET_MENU_HPP_INCLUDED
#define WORLD_SET_MENU_HPP_INCLUDED

#include "gui/menu.hpp"

enum WorldSetMenuIDs
{
  WORLDSET_STORY,
  WORLDSET_CONTRIB
};

class WorldSetMenu : public Menu
{
public:
  WorldSetMenu();

  void menu_action(MenuItem* item) override;
};


#endif // WORLD_SET_MENU_HPP_INCLUDED

// EOF
