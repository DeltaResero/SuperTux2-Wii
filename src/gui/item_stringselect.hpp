// src/gui/item_stringselect.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
//  SuperTux
//  Copyright (C) 2015 Hume2 <teratux.mail@gmail.com>
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef HEADER_SUPERTUX_GUI_ITEM_STRINGSELECT_HPP
#define HEADER_SUPERTUX_GUI_ITEM_STRINGSELECT_HPP

#include <list>
#include <memory>
#include <SDL.h>

#include "gui/menu_item.hpp"

class ItemStringSelect : public MenuItem
{
  public:
    ItemStringSelect(const std::string& text_, const std::vector<std::string>& list_, int* selected_, int _id = -1);

    /** Draws the menu item. */
    virtual void draw(DrawingContext&, Vector pos, int menu_width, float value_width, bool active);

    /** Returns the minimum width of the menu item. */
    virtual int get_width() const;

    /** Width of the longest value this setting can take. */
    virtual float get_value_width() const;

    /** Processes the menu action. */
    virtual void process_action(MenuAction action);

    /** Steps the value when an arrow was clicked, and does nothing when the
        click landed anywhere else, since there is nothing else to press. */
    virtual MenuAction get_click_action(float x_offset, int menu_width,
                                        float value_width) const;

    std::vector<std::string> list; // list of values for a STRINGSELECT item
    int* selected; // currently selected item

  private:
    /** Width of the longest value in the list, which is what the row is laid
        out around so that stepping through the values moves nothing. */
    float widest_value;

  private:
    ItemStringSelect(const ItemStringSelect&);
    ItemStringSelect& operator=(const ItemStringSelect&);
};

#endif

/* EOF */
