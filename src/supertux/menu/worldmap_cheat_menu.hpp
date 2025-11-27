// src/supertux/menu/worldmap_cheat_menu.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2014 Ingo Ruhnke <grumbel@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_SUPERTUX_MENU_WORLDMAP_CHEAT_MENU_HPP
#define HEADER_SUPERTUX_SUPERTUX_MENU_WORLDMAP_CHEAT_MENU_HPP

#include "gui/menu.hpp"

class WorldmapCheatMenu : public Menu
{
private:
private:
  enum MenuIDs {
    MNID_GROW,
    MNID_FIRE,
    MNID_ICE,
    MNID_AIR,
    MNID_EARTH,
    MNID_SHRINK,
    MNID_FINISH_LEVEL,
    MNID_RESET_LEVEL,
    MNID_FINISH_WORLDMAP,
    MNID_RESET_WORLDMAP,
    MNID_MOVE_TO_MAIN
  };

public:
  WorldmapCheatMenu();

  void menu_action(MenuItem* item) override;

private:
  WorldmapCheatMenu(const WorldmapCheatMenu&) = delete;
  WorldmapCheatMenu& operator=(const WorldmapCheatMenu&) = delete;
};

#endif // HEADER_SUPERTUX_SUPERTUX_MENU_WORLDMAP_CHEAT_MENU_HPP

// EOF
