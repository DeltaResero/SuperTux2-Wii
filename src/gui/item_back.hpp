// src/gui/item_back.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2015 Hume2 <teratux.mail@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_GUI_ITEM_BACK_HPP
#define HEADER_SUPERTUX_GUI_ITEM_BACK_HPP

#include <list>
#include <memory>
#include <SDL.h>

#include "gui/menu_item.hpp"

class ItemBack : public MenuItem
{
  public:
    ItemBack(const std::string& text_, int id = -1);

    /** Draws the menu item. */
    virtual void draw(DrawingContext&, Vector pos, int menu_width, bool active);

    /** Returns the minimum width of the menu item. */
    virtual int get_width() const;

    /** Processes the menu action. */
    virtual void process_action(MenuAction action);

    /** Returns true when the memu manager shouldn't do anything else. */
    virtual bool no_other_action() const {
      return true;
    }

  private:
    ItemBack(const ItemBack&);
    ItemBack& operator=(const ItemBack&);
};

#endif // HEADER_SUPERTUX_GUI_ITEM_BACK_HPP

// EOF
