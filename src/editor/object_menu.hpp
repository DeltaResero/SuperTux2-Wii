// src/editor/object_menu.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2015 Hume2 <teratux.mail@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_EDITOR_OBJECT_MENU_HPP
#define HEADER_SUPERTUX_EDITOR_OBJECT_MENU_HPP

#include "gui/menu.hpp"

class GameObject;

class ObjectMenu : public Menu
{
  public:
    ObjectMenu(GameObject *go);
    ~ObjectMenu();

    void menu_action(MenuItem* item) override;

    GameObject *object;

  private:
    enum MenuIDs {
      MNID_REMOVE
    };

    ObjectMenu(const ObjectMenu&);
    ObjectMenu& operator=(const ObjectMenu&);
};

#endif // HEADER_SUPERTUX_EDITOR_OBJECT_MENU_HPP

// EOF
