// src/supertux/menu/editor_level_select_menu.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2015 Hume2 <teratux.mail@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_SUPERTUX_MENU_EDITOR_LEVEL_SELECT_MENU_HPP
#define HEADER_SUPERTUX_SUPERTUX_MENU_EDITOR_LEVEL_SELECT_MENU_HPP

#include "gui/menu.hpp"

class Levelset;
class World;

class EditorLevelSelectMenu : public Menu
{
private:
  std::unique_ptr<Levelset> m_levelset;

public:
  EditorLevelSelectMenu();
  EditorLevelSelectMenu(std::unique_ptr<World> world);
  ~EditorLevelSelectMenu();

  void menu_action(MenuItem* item) override;

private:
  void initialize();
  void create_level();

  EditorLevelSelectMenu(const EditorLevelSelectMenu&);
  EditorLevelSelectMenu& operator=(const EditorLevelSelectMenu&);
};

#endif // HEADER_SUPERTUX_SUPERTUX_MENU_EDITOR_LEVEL_SELECT_MENU_HPP

// EOF
