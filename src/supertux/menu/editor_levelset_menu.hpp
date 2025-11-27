// src/supertux/menu/editor_levelset_menu.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2016 Hume2 <teratux.mail@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_SUPERTUX_MENU_EDITOR_LEVELSET_MENU_HPP
#define HEADER_SUPERTUX_SUPERTUX_MENU_EDITOR_LEVELSET_MENU_HPP

#include "gui/menu.hpp"

class World;

class EditorLevelsetMenu : public Menu
{
private:
public:
  EditorLevelsetMenu();
  EditorLevelsetMenu(World* world_);
  ~EditorLevelsetMenu();

  void menu_action(MenuItem* item) override;

private:
  enum MenuIDs {
    MNID_EDITWORLDMAP,
    MNID_NEWWORLDMAP
  };

  void initialize();
  void create_worldmap();

  World* world;

  EditorLevelsetMenu(const EditorLevelsetMenu&);
  EditorLevelsetMenu& operator=(const EditorLevelsetMenu&);
};

#endif // HEADER_SUPERTUX_SUPERTUX_MENU_EDITOR_LEVELSET_MENU_HPP

// EOF
