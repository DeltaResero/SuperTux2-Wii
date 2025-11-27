// src/gui/item_goto.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2015 Hume2 <teratux.mail@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_GUI_ITEM_GOTO_HPP
#define HEADER_SUPERTUX_GUI_ITEM_GOTO_HPP

#include <list>
#include <memory>
#include <SDL.h>

#include "gui/menu_item.hpp"

class ItemGoTo : public MenuItem
{
  public:
    ItemGoTo(const std::string& text_, int target_menu_, int id = -1);

    /** Processes the menu action. */
    virtual void process_action(MenuAction action);

    int target_menu;

  private:
    ItemGoTo(const ItemGoTo&);
    ItemGoTo& operator=(const ItemGoTo&);
};

#endif // HEADER_SUPERTUX_GUI_ITEM_GOTO_HPP

// EOF
