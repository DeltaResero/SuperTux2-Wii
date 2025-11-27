// src/gui/item_badguy_select.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2016 Hume2 <teratux.mail@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_GUI_ITEM_BADGUY_SELECT_HPP
#define HEADER_SUPERTUX_GUI_ITEM_BADGUY_SELECT_HPP

#include "gui/menu_item.hpp"

#include "gui/menu.hpp"

class ItemBadguySelect : public MenuItem
{
  public:
    ItemBadguySelect(const std::string& text_, std::vector<std::string>* badguys_, int id = -1);

    /** Processes the menu action. */
    virtual void process_action(MenuAction action);

  private:
    std::vector<std::string>* badguys;

    ItemBadguySelect(const ItemBadguySelect&);
    ItemBadguySelect& operator=(const ItemBadguySelect&);
};

#endif // HEADER_SUPERTUX_GUI_ITEM_BADGUY_SELECT_HPP

// EOF
