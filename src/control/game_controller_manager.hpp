// src/control/game_controller_manager.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2014 Ingo Ruhnke <grumbel@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_CONTROL_GAME_CONTROLLER_MANAGER_HPP
#define HEADER_SUPERTUX_CONTROL_GAME_CONTROLLER_MANAGER_HPP

#include <vector>
#include <array>

#include "SDL.h"

#include "control/controller.hpp"

class InputManager;

class GameControllerManager
{
private:
  InputManager* m_parent;
  int m_deadzone;
  std::vector<SDL_GameController*> m_game_controllers;
  std::array<bool, Controller::CONTROLCOUNT> m_stick_state;
  std::array<bool, Controller::CONTROLCOUNT> m_button_state;

public:
  GameControllerManager(InputManager* parent);
  ~GameControllerManager();

  void process_button_event(const SDL_ControllerButtonEvent& ev);
  void process_axis_event(const SDL_ControllerAxisEvent& ev);

  void on_controller_added(int joystick_index);
  void on_controller_removed(int instance_id);

private:
  GameControllerManager(const GameControllerManager&) = delete;
  GameControllerManager& operator=(const GameControllerManager&) = delete;
};

#endif // HEADER_SUPERTUX_CONTROL_GAME_CONTROLLER_MANAGER_HPP

// EOF
