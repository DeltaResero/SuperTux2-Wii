// src/gui/item_file.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2016 Hume2 <teratux.mail@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_GUI_ITEM_FILE_HPP
#define HEADER_SUPERTUX_GUI_ITEM_FILE_HPP

#include <list>
#include <memory>
#include <SDL.h>

#include "gui/menu_item.hpp"

#include "gui/menu.hpp"

class ItemFile : public MenuItem
{
  public:
    ItemFile(const std::string& text_, std::string* filename_,
             const std::vector<std::string>& extensions_, int _id = -1);

    /** Processes the menu action. */
    virtual void process_action(MenuAction action);

  private:
    std::string* filename;
    std::vector<std::string> extensions;

    ItemFile(const ItemFile&);
    ItemFile& operator=(const ItemFile&);
};

#endif // HEADER_SUPERTUX_GUI_ITEM_FILE_HPP

// EOF
