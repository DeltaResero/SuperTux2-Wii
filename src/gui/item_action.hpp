// src/gui/item_action.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2015 Hume2 <teratux.mail@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_GUI_ITEM_ACTION_HPP
#define HEADER_SUPERTUX_GUI_ITEM_ACTION_HPP

#include <list>
#include <memory>
#include <SDL.h>

#include "gui/menu_item.hpp"

class ItemAction : public MenuItem
{
  public:
    ItemAction(const std::string& text_, int id = -1);

  private:
    ItemAction(const ItemAction&);
    ItemAction& operator=(const ItemAction&);
};

#endif // HEADER_SUPERTUX_GUI_ITEM_ACTION_HPP

// EOF
