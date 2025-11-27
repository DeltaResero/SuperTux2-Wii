// src/supertux/menu/editor_objectgroup_menu.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2015 Hume2 <teratux.mail@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_SUPERTUX_MENU_EDITOR_OBJECTGROUP_MENU_HPP
#define HEADER_SUPERTUX_SUPERTUX_MENU_EDITOR_OBJECTGROUP_MENU_HPP

#include "gui/menu.hpp"

class EditorObjectgroupMenu : public Menu
{
public:
  EditorObjectgroupMenu();
  ~EditorObjectgroupMenu();

  void menu_action(MenuItem* item) override;

private:
  EditorObjectgroupMenu(const EditorObjectgroupMenu&);
  EditorObjectgroupMenu& operator=(const EditorObjectgroupMenu&);
};

#endif // HEADER_SUPERTUX_SUPERTUX_MENU_EDITOR_OBJECTGROUP_MENU_HPP

// EOF
