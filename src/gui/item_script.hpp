// src/gui/item_script.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2016 Hume2 <teratux.mail@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_GUI_ITEM_SCRIPT_HPP
#define HEADER_SUPERTUX_GUI_ITEM_SCRIPT_HPP

#include <list>
#include <memory>
#include <SDL.h>

#include "gui/menu_item.hpp"

#include "gui/menu.hpp"

class ItemScript : public MenuItem
{
  public:
    ItemScript(const std::string& text_, std::string* script_, int id = -1);

    /** Processes the menu action. */
    virtual void process_action(MenuAction action);

  private:
    std::string* script;

    ItemScript(const ItemScript&);
    ItemScript& operator=(const ItemScript&);
};

#endif // HEADER_SUPERTUX_GUI_ITEM_SCRIPT_HPP

// EOF
