// src/supertux/menu/editor_new_levelset_menu.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2016 Hume2 <teratux.mail@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_SUPERTUX_MENU_EDITOR_NEW_LEVELSET_MENU_HPP
#define HEADER_SUPERTUX_SUPERTUX_MENU_EDITOR_NEW_LEVELSET_MENU_HPP

#include "gui/menu.hpp"

class EditorNewLevelsetMenu;
class World;

class EditorNewLevelsetMenu : public Menu
{
public:
  EditorNewLevelsetMenu();

  void menu_action(MenuItem* item) override;

private:
  std::string levelset_name;
  std::string levelset_desc;

  EditorNewLevelsetMenu(const EditorNewLevelsetMenu&);
  EditorNewLevelsetMenu& operator=(const EditorNewLevelsetMenu&);
};

#endif // HEADER_SUPERTUX_SUPERTUX_MENU_EDITOR_NEW_LEVELSET_MENU_HPP

// EOF
