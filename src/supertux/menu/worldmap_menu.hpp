// src/supertux/menu/worldmap_menu.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2009 Ingo Ruhnke <grumbel@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_SUPERTUX_MENU_WORLDMAP_MENU_HPP
#define HEADER_SUPERTUX_SUPERTUX_MENU_WORLDMAP_MENU_HPP

#include "gui/menu.hpp"

enum WorldMapMenuIDs {
  MNID_RETURNWORLDMAP,
  MNID_QUITWORLDMAP
};

class WorldmapMenu : public Menu
{
private:
public:
  WorldmapMenu();

  void menu_action(MenuItem* item) override;

private:
  WorldmapMenu(const WorldmapMenu&);
  WorldmapMenu& operator=(const WorldmapMenu&);
};

#endif // HEADER_SUPERTUX_SUPERTUX_MENU_WORLDMAP_MENU_HPP

// EOF
