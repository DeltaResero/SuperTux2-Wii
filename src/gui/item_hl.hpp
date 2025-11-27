// src/gui/item_hl.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2015 Hume2 <teratux.mail@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_GUI_ITEM_HL_HPP
#define HEADER_SUPERTUX_GUI_ITEM_HL_HPP

#include <list>
#include <memory>
#include <SDL.h>

#include "gui/menu_item.hpp"

class ItemHorizontalLine : public MenuItem
{
  public:
    ItemHorizontalLine();

    /** Draws the menu item. */
    virtual void draw(DrawingContext&, Vector pos, int menu_width, bool active);

    /** Returns true when the menu item has no action and therefore can be skipped.
        Useful for labels and horizontal lines.*/
    virtual bool skippable() const {
      return true;
    }

    /** Returns the minimum width of the menu item. */
    virtual int get_width() const;

  private:
    ItemHorizontalLine(const ItemHorizontalLine&);
    ItemHorizontalLine& operator=(const ItemHorizontalLine&);
};

#endif // HEADER_SUPERTUX_GUI_ITEM_HL_HPP

// EOF
