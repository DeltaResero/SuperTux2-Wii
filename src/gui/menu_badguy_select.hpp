// src/gui/menu_badguy_select.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2016 Hume2 <teratux.mail@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_GUI_MENU_BADGUY_SELECT_HPP
#define HEADER_SUPERTUX_GUI_MENU_BADGUY_SELECT_HPP

#include "gui/menu.hpp"

class BadguySelectMenu : public Menu
{
public:
  BadguySelectMenu(std::vector<std::string>* badguys_);
  ~BadguySelectMenu();

  void menu_action(MenuItem* item) override;

  static std::vector<std::string> all_badguys;

  void remove_badguy();

private:
  std::vector<std::string>* badguys;
  int selected;
  int remove_item;

  void refresh_menu();
  void add_badguy();

  BadguySelectMenu(const BadguySelectMenu&);
  BadguySelectMenu& operator=(const BadguySelectMenu&);
};

#endif // HEADER_SUPERTUX_GUI_MENU_BADGUY_SELECT_HPP

// EOF
