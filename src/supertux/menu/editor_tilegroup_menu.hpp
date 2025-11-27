// src/supertux/menu/editor_tilegroup_menu.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2015 Hume2 <teratux.mail@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_SUPERTUX_MENU_EDITOR_TILEGROUP_MENU_HPP
#define HEADER_SUPERTUX_SUPERTUX_MENU_EDITOR_TILEGROUP_MENU_HPP

#include "gui/menu.hpp"

class EditorTilegroupMenu : public Menu
{
public:
  EditorTilegroupMenu();
  ~EditorTilegroupMenu();

  void menu_action(MenuItem* item) override;

private:
  EditorTilegroupMenu(const EditorTilegroupMenu&);
  EditorTilegroupMenu& operator=(const EditorTilegroupMenu&);
};

#endif // HEADER_SUPERTUX_SUPERTUX_MENU_EDITOR_TILEGROUP_MENU_HPP

// EOF
