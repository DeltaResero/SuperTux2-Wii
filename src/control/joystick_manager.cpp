// src/control/joystick_manager.cpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
//  SuperTux
//  Copyright (C) 2014 Ingo Ruhnke <grumbel@gmail.com>
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

#include "control/joystick_manager.hpp"

#include <iostream>
#include <algorithm>
#include <map>
#include <utility>

#include "control/input_manager.hpp"
#include "control/joystick_config.hpp"
#include "gui/menu_manager.hpp"
#include "supertux/menu/joystick_menu.hpp"
#include "util/log.hpp"
#include "util/wii.hpp"

namespace {

/* Every mapping sits under one id. The config format records no joystick,
   and SDL hands out a fresh id each time a pad is added back. */
const JoystickConfig::JoyId JOY_ID = 0;

#ifdef __wii__

/* A quarter turn anticlockwise, matching what SDL does for a remote held
   sideways. Applied here as SDL settles that once, before libogc has had a
   chance to report the nunchuk the choice depends on. */
Uint8 turn_hat_sideways(Uint8 value)
{
  Uint8 turned = SDL_HAT_CENTERED;

  if (value & SDL_HAT_UP)    turned |= SDL_HAT_LEFT;
  if (value & SDL_HAT_DOWN)  turned |= SDL_HAT_RIGHT;
  if (value & SDL_HAT_LEFT)  turned |= SDL_HAT_DOWN;
  if (value & SDL_HAT_RIGHT) turned |= SDL_HAT_UP;

  return turned;
}

/* SDL calls a GameCube pad "Gamecube N" and everything on a remote
   "Wiimote N". Anything it cannot name is taken for a remote. */
bool is_gamecube_pad(SDL_JoystickID which)
{
  SDL_Joystick* joystick = SDL_JoystickFromInstanceID(which);
  const char* name = joystick ? SDL_JoystickName(joystick) : nullptr;

  return name && SDL_strncmp(name, "Gamecube", 8) == 0;
}

#endif

} // namespace

JoystickManager::JoystickManager(InputManager* parent_,
                                 JoystickConfig& joystick_config) :
  parent(parent_),
  m_joystick_config(joystick_config),
  min_joybuttons(),
  max_joybuttons(),
  max_joyaxis(),
  max_joyhats(),
  hat_state(),
  wait_for_joystick(-1),
  joysticks()
{
}

JoystickManager::~JoystickManager()
{
  for(auto& joy : joysticks)
  {
    SDL_JoystickClose(joy);
  }
}

void
JoystickManager::on_joystick_added(int joystick_index)
{
  log_debug << "on_joystick_added(): " << joystick_index << std::endl;
  SDL_Joystick* joystick = SDL_JoystickOpen(joystick_index);
  if (!joystick)
  {
    log_warning << "failed to open joystick: " << joystick_index
                << ": " << SDL_GetError() << std::endl;
  }
  else
  {
    joysticks.push_back(joystick);
  }

  if(min_joybuttons < 0 || SDL_JoystickNumButtons(joystick) < min_joybuttons)
    min_joybuttons = SDL_JoystickNumButtons(joystick);

  if(SDL_JoystickNumButtons(joystick) > max_joybuttons)
    max_joybuttons = SDL_JoystickNumButtons(joystick);

  if(SDL_JoystickNumAxes(joystick) > max_joyaxis)
    max_joyaxis = SDL_JoystickNumAxes(joystick);

  if(SDL_JoystickNumHats(joystick) > max_joyhats)
    max_joyhats = SDL_JoystickNumHats(joystick);
}

void
JoystickManager::on_joystick_removed(int instance_id)
{
  log_debug << "on_joystick_removed: " << static_cast<int>(instance_id) << std::endl;
  for(auto& joy : joysticks)
  {
    SDL_JoystickID id = SDL_JoystickInstanceID(joy);
    if (id == instance_id)
    {
      SDL_JoystickClose(joy);
      joy = nullptr;
    }
  }

  joysticks.erase(std::remove(joysticks.begin(), joysticks.end(), nullptr),
                  joysticks.end());

  hat_state.erase(instance_id);
}

void
JoystickManager::process_hat_event(const SDL_JoyHatEvent& jhat)
{
  Uint8 value = jhat.value;
#ifdef __wii__
  /* The remote lies sideways on its own and upright once anything is in its
     expansion port, so the D-pad turns with it. A GameCube pad never turns,
     and has_expansion only speaks for the remote. */
  if (!is_gamecube_pad(jhat.which) && !Wii::has_expansion())
    value = turn_hat_sideways(value);
#endif

  Uint8& state = hat_state[jhat.which];
  Uint8 changed = state ^ value;

  if (wait_for_joystick >= 0)
  {
    if (changed & SDL_HAT_UP && value & SDL_HAT_UP)
      m_joystick_config.bind_joyhat(JOY_ID, SDL_HAT_UP, Controller::Control(wait_for_joystick));

    if (changed & SDL_HAT_DOWN && value & SDL_HAT_DOWN)
      m_joystick_config.bind_joyhat(JOY_ID, SDL_HAT_DOWN, Controller::Control(wait_for_joystick));

    if (changed & SDL_HAT_LEFT && value & SDL_HAT_LEFT)
      m_joystick_config.bind_joyhat(JOY_ID, SDL_HAT_LEFT, Controller::Control(wait_for_joystick));

    if (changed & SDL_HAT_RIGHT && value & SDL_HAT_RIGHT)
      m_joystick_config.bind_joyhat(JOY_ID, SDL_HAT_RIGHT, Controller::Control(wait_for_joystick));

    MenuManager::instance().refresh();
    wait_for_joystick = -1;
  }
  else
  {
    if (changed & SDL_HAT_UP)
    {
      JoystickConfig::HatMap::iterator it = m_joystick_config.joy_hat_map.find(std::make_pair(JOY_ID, SDL_HAT_UP));
      if (it != m_joystick_config.joy_hat_map.end())
        set_joy_controls(it->second, value & SDL_HAT_UP);
    }

    if (changed & SDL_HAT_DOWN)
    {
      JoystickConfig::HatMap::iterator it = m_joystick_config.joy_hat_map.find(std::make_pair(JOY_ID, SDL_HAT_DOWN));
      if (it != m_joystick_config.joy_hat_map.end())
        set_joy_controls(it->second, value & SDL_HAT_DOWN);
    }

    if (changed & SDL_HAT_LEFT)
    {
      JoystickConfig::HatMap::iterator it = m_joystick_config.joy_hat_map.find(std::make_pair(JOY_ID, SDL_HAT_LEFT));
      if (it != m_joystick_config.joy_hat_map.end())
        set_joy_controls(it->second, value & SDL_HAT_LEFT);
    }

    if (changed & SDL_HAT_RIGHT)
    {
      JoystickConfig::HatMap::iterator it = m_joystick_config.joy_hat_map.find(std::make_pair(JOY_ID, SDL_HAT_RIGHT));
      if (it != m_joystick_config.joy_hat_map.end())
        set_joy_controls(it->second, value & SDL_HAT_RIGHT);
    }
  }

  state = value;
}

void
JoystickManager::process_axis_event(const SDL_JoyAxisEvent& jaxis)
{
  if (wait_for_joystick >= 0)
  {
    if (abs(jaxis.value) > m_joystick_config.dead_zone) {
      if (jaxis.value < 0)
        m_joystick_config.bind_joyaxis(JOY_ID, -(jaxis.axis + 1), Controller::Control(wait_for_joystick));
      else
        m_joystick_config.bind_joyaxis(JOY_ID, jaxis.axis + 1, Controller::Control(wait_for_joystick));

      MenuManager::instance().refresh();
      wait_for_joystick = -1;
    }
  }
  else
  {
    // Split the axis into left and right, so that both can be
    // mapped separately (needed for jump/down vs up/down)
    int axis = jaxis.axis + 1;

    auto left = m_joystick_config.joy_axis_map.find(std::make_pair(JOY_ID, -axis));
    auto right = m_joystick_config.joy_axis_map.find(std::make_pair(JOY_ID, axis));

    if(left == m_joystick_config.joy_axis_map.end()) {
      // std::cout << "Unmapped joyaxis " << (int)jaxis.axis << " moved" << std::endl;
    } else {
      if (jaxis.value < -m_joystick_config.dead_zone)
        set_joy_controls(left->second,  true);
      else
        set_joy_controls(left->second, false);
    }

    if(right == m_joystick_config.joy_axis_map.end()) {
      // std::cout << "Unmapped joyaxis " << (int)jaxis.axis << " moved" << std::endl;
    } else {
      if (jaxis.value > m_joystick_config.dead_zone)
        set_joy_controls(right->second, true);
      else
        set_joy_controls(right->second, false);
    }
  }
}

void
JoystickManager::process_button_event(const SDL_JoyButtonEvent& jbutton)
{
  if(wait_for_joystick >= 0)
  {
    if(jbutton.state == SDL_PRESSED)
    {
      m_joystick_config.bind_joybutton(JOY_ID, jbutton.button, (Controller::Control)wait_for_joystick);
      MenuManager::instance().refresh();
      parent->reset();
      wait_for_joystick = -1;
    }
  }
  else
  {
    auto i = m_joystick_config.joy_button_map.find(std::make_pair(JOY_ID, jbutton.button));
    if(i == m_joystick_config.joy_button_map.end()) {
      log_debug << "Unmapped joybutton " << (int)jbutton.button << " pressed" << std::endl;
    } else {
      set_joy_controls(i->second, (jbutton.state == SDL_PRESSED));
    }
  }
}

void
JoystickManager::bind_next_event_to(Controller::Control id)
{
  wait_for_joystick = id;
}

void
JoystickManager::set_joy_controls(Controller::Control id, bool value)
{
  if (m_joystick_config.jump_with_up_joy &&
      id == Controller::UP)
  {
    parent->get_controller()->set_control(Controller::JUMP, value);
  }

#ifdef __wii__
  /* The remote has too few buttons to spare any for the menus, so jump and
     action carry a menu control too. Menu::process_input reads back after
     hit, so action cancels rather than confirming. */
  if (id == Controller::JUMP)
  {
    parent->get_controller()->set_control(Controller::MENU_SELECT, value);
  }
  else if (id == Controller::ACTION)
  {
    parent->get_controller()->set_control(Controller::MENU_BACK, value);
  }
#endif

  parent->get_controller()->set_control(id, value);
}

/* EOF */
