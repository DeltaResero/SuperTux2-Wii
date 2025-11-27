// src/supertux/menu/editor_levelset_select_menu.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2015 Hume2 <teratux.mail@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_SUPERTUX_MENU_EDITOR_LEVELSET_SELECT_MENU_HPP
#define HEADER_SUPERTUX_SUPERTUX_MENU_EDITOR_LEVELSET_SELECT_MENU_HPP

#include "gui/menu.hpp"

class EditorLevelsetSelectMenu;
class World;

class EditorLevelsetSelectMenu : public Menu
{
private:
  std::vector<std::string> m_contrib_worlds;

public:
  EditorLevelsetSelectMenu();
  ~EditorLevelsetSelectMenu();

  void menu_action(MenuItem* item) override;

private:
  EditorLevelsetSelectMenu(const EditorLevelsetSelectMenu&);
  EditorLevelsetSelectMenu& operator=(const EditorLevelsetSelectMenu&);
};

#endif // HEADER_SUPERTUX_SUPERTUX_MENU_EDITOR_LEVELSET_SELECT_MENU_HPP

// EOF
