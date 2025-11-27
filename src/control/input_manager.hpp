// src/control/input_manager.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_CONTROL_INPUT_MANAGER_HPP
#define HEADER_SUPERTUX_CONTROL_INPUT_MANAGER_HPP

#include "control/controller.hpp"

#include <SDL.h>
#include <map>
#include <string>
#include <vector>
#include <memory>

#include "util/currenton.hpp"

class GameControllerManager;
class JoystickManager;
class JoystickMenu;
class KeyboardManager;
class KeyboardMenu;
class KeyboardConfig;
class JoystickConfig;

class InputManager final : public Currenton<InputManager>
{
private:
  friend class KeyboardMenu;
  friend class JoystickMenu;

  typedef Controller::Control Control;

public:
  InputManager(KeyboardConfig& keyboard_config,
               JoystickConfig& joystick_config);
  virtual ~InputManager();

  void process_event(const SDL_Event& event);

  void update();
  void reset();

  void use_game_controller(bool v);
  bool use_game_controller() const { return m_use_game_controller; }

  Controller* get_controller() const;

private:
  std::unique_ptr<Controller> controller;

public:
  bool& m_use_game_controller;
  std::unique_ptr<KeyboardManager> keyboard_manager;
  std::unique_ptr<JoystickManager> joystick_manager;
  std::unique_ptr<GameControllerManager> game_controller_manager;

private:
  InputManager(const InputManager&);
  InputManager& operator=(const InputManager&);
};

#endif // HEADER_SUPERTUX_CONTROL_INPUT_MANAGER_HPP

// EOF
