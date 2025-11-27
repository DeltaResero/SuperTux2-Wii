// src/supertux/menu/cheat_menu.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2014 Ingo Ruhnke <grumbel@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_SUPERTUX_MENU_CHEAT_MENU_HPP
#define HEADER_SUPERTUX_SUPERTUX_MENU_CHEAT_MENU_HPP

#include "gui/menu.hpp"

class CheatMenu : public Menu
{
private:
  enum MenuIDs {
    MNID_GROW,
    MNID_FIRE,
    MNID_ICE,
    MNID_AIR,
    MNID_EARTH,
    MNID_STAR,
    MNID_SHRINK,
    MNID_KILL,
    MNID_FINISH,
    MNID_GHOST
  };

public:
  CheatMenu();

  void menu_action(MenuItem* item) override;

private:
  CheatMenu(const CheatMenu&) = delete;
  CheatMenu& operator=(const CheatMenu&) = delete;
};

#endif // HEADER_SUPERTUX_SUPERTUX_MENU_CHEAT_MENU_HPP

// EOF
