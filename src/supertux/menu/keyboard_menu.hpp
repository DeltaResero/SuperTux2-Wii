// src/supertux/menu/keyboard_menu.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>,
// Copyright (C) 2007 Ingo Ruhnke <grumbel@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_CONTROL_KEYBOARD_MENU_HPP
#define HEADER_SUPERTUX_CONTROL_KEYBOARD_MENU_HPP

#include "control/input_manager.hpp"
#include "gui/menu_item.hpp"

class KeyboardMenu : public Menu
{
private:
  InputManager& m_input_manager;

public:
  KeyboardMenu(InputManager& input_manager);
  ~KeyboardMenu();

  void refresh() override;
  std::string get_key_name(SDL_Keycode key) const;
  void menu_action(MenuItem* item) override;

private:
  KeyboardMenu(const KeyboardMenu&);
  KeyboardMenu& operator=(const KeyboardMenu&);
};

#endif // HEADER_SUPERTUX_CONTROL_KEYBOARD_MENU_HPP

// EOF
