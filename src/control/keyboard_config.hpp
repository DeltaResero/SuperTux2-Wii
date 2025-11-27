// src/control/keyboard_config.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2014 Ingo Ruhnke <grumbel@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_CONTROL_KEYBOARD_CONFIG_HPP
#define HEADER_SUPERTUX_CONTROL_KEYBOARD_CONFIG_HPP

#include <SDL.h>
#include <map>

#include "control/controller.hpp"
#include "util/writer.hpp"

class ReaderMapping;

class KeyboardConfig
{
public:
  KeyboardConfig();

  SDL_Keycode reversemap_key(Controller::Control c) const;
  void bind_key(SDL_Keycode key, Controller::Control c);

  void read(const ReaderMapping& keymap_lisp);
  void write(Writer& writer);

  typedef std::map<SDL_Keycode, Controller::Control> KeyMap;
  KeyMap keymap;
  bool jump_with_up_kbd;
};

#endif // HEADER_SUPERTUX_CONTROL_KEYBOARD_CONFIG_HPP

// EOF
