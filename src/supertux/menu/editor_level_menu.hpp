// src/supertux/menu/editor_level_menu.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2016 Hume2 <teratux.mail@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_SUPERTUX_MENU_EDITOR_LEVEL_MENU_HPP
#define HEADER_SUPERTUX_SUPERTUX_MENU_EDITOR_LEVEL_MENU_HPP

#include "gui/menu.hpp"

class EditorLevelMenu : public Menu
{
private:
public:
  EditorLevelMenu();
  ~EditorLevelMenu();

  void menu_action(MenuItem* item) override;
  bool on_back_action() override;
  bool is_sensitive() const override {
    return true;
  }

private:
  std::string old_tileset;

  EditorLevelMenu(const EditorLevelMenu&);
  EditorLevelMenu& operator=(const EditorLevelMenu&);
};

#endif // HEADER_SUPERTUX_SUPERTUX_MENU_EDITOR_LEVEL_MENU_HPP

// EOF
