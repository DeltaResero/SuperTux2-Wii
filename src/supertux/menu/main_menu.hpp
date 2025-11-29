// src/supertux/menu/main_menu.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2009 Ingo Ruhnke <grumbel@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_SUPERTUX_MENU_MAIN_MENU_HPP
#define HEADER_SUPERTUX_SUPERTUX_MENU_MAIN_MENU_HPP

#include "gui/menu.hpp"

class AddonMenu;
class WorldSetMenu;

enum MainMenuIDs {
  MNID_STARTGAME,
  MNID_OPTIONMENU,
  MNID_LEVELEDITOR,
  MNID_CREDITS,
  MNID_QUITMAINMENU
};

class MainMenu : public Menu
{
public:
  MainMenu();

  void on_window_resize() override;
  void menu_action(MenuItem* item) override;

private:
  MainMenu(const MainMenu&);
  MainMenu& operator=(const MainMenu&);
};

#endif // HEADER_SUPERTUX_SUPERTUX_MENU_MAIN_MENU_HPP

// EOF
