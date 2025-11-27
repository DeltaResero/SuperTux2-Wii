// src/control/keyboard_manager.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>,
// Copyright (C) 2007-2014 Ingo Ruhnke <grumbel@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_CONTROL_KEYBOARD_MANAGER_HPP
#define HEADER_SUPERTUX_CONTROL_KEYBOARD_MANAGER_HPP

#include <map>

#include "SDL.h"

#include "control/controller.hpp"

class InputManager;
class KeyboardConfig;

class KeyboardManager final
{
private:
  InputManager* m_parent;
  KeyboardConfig& m_keyboard_config;
  int wait_for_key;
  bool m_lock_text_input;

public:
  KeyboardManager(InputManager* parent, KeyboardConfig& keyboard_config);
  ~KeyboardManager();

  void process_key_event(const SDL_KeyboardEvent& event);
  void process_text_input_event(const SDL_TextInputEvent& event);
  void process_console_key_event(const SDL_KeyboardEvent& event);
  void process_menu_key_event(const SDL_KeyboardEvent& event);

  void bind_next_event_to(Controller::Control id);

private:
  KeyboardManager(const KeyboardManager&) = delete;
  KeyboardManager& operator=(const KeyboardManager&) = delete;
};

#endif // HEADER_SUPERTUX_CONTROL_KEYBOARD_MANAGER_HPP

// EOF
