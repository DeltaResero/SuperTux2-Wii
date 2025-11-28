// src/supertux/menu/joystick_menu.cpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>,
// Copyright (C) 2007,2014 Ingo Ruhnke <grumbel@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#include "supertux/menu/joystick_menu.hpp"

#include <sstream>

#include "control/joystick_manager.hpp"
#include "gui/item_controlfield.hpp"
#include "supertux/gameconfig.hpp"
#include "supertux/globals.hpp"
#include "util/gettext.hpp"

namespace {

enum {
  MNID_JUMP_WITH_UP = Controller::CONTROLCOUNT,
  MNID_SCAN_JOYSTICKS,
  MNID_AUTO_JOYSTICK_CFG
};

} // namespace

JoystickMenu::JoystickMenu(InputManager& input_manager) :
  m_input_manager(input_manager),
  m_joysticks_available(false),
  m_auto_joystick_cfg(!m_input_manager.use_game_controller())
{
  recreate_menu();
}

JoystickMenu::~JoystickMenu()
{}

void
JoystickMenu::recreate_menu()
{
  clear();
  add_label("Setup Joystick");
  add_hl();

  add_toggle(MNID_AUTO_JOYSTICK_CFG, "Manual Configuration",
             &m_auto_joystick_cfg)
    ->set_help("Use manual configuration instead of SDL2's automatic GameController support");

  if (m_input_manager.use_game_controller())
  {
    m_joysticks_available = false;
  }
  else
  {
    if (m_input_manager.joystick_manager->get_num_joysticks() > 0)
    {
      m_joysticks_available = true;

      add_controlfield(Controller::UP,          "Up");
      add_controlfield(Controller::DOWN,        "Down");
      add_controlfield(Controller::LEFT,        "Left");
      add_controlfield(Controller::RIGHT,       "Right");
      add_controlfield(Controller::JUMP,        "Jump");
      add_controlfield(Controller::ACTION,      "Action");
      add_controlfield(Controller::START,       "Pause/Menu");
      add_controlfield(Controller::PEEK_LEFT,   "Peek Left");
      add_controlfield(Controller::PEEK_RIGHT,  "Peek Right");
      add_controlfield(Controller::PEEK_UP,     "Peek Up");
      add_controlfield(Controller::PEEK_DOWN,   "Peek Down");
      if (g_config->developer_mode) {
        add_controlfield(Controller::CONSOLE, "Console");
      }
      if (g_config->developer_mode) {
        add_controlfield(Controller::CHEAT_MENU, "Cheat Menu");
      }
      add_hl();
      add_inactive("The following feature is deprecated.");
      add_inactive("It will be removed from the next release");
      add_inactive("of SuperTux.");
      add_toggle(MNID_JUMP_WITH_UP, "Jump with Up", &g_config->joystick_config.jump_with_up_joy);
    }
    else
    {
      m_joysticks_available = false;

      add_inactive("No Joysticks found");
      add_entry(MNID_SCAN_JOYSTICKS, "Scan for Joysticks");
    }
  }

  add_hl();
  add_back("Back");
  refresh();
}

std::string
JoystickMenu::get_button_name(int button) const
{
  if(button < 0)
  {
    return "None";
  }
  else
  {
    std::ostringstream name;
    name << "Button " << button;
    return name.str();
  }
}

void
JoystickMenu::menu_action(MenuItem* item)
{
  if (0 <= item->id && item->id < Controller::CONTROLCOUNT)
  {
    ItemControlField* micf = dynamic_cast<ItemControlField*>(item);
    if (!micf) {
      return;
    }
    micf->change_input("Press Button");
    m_input_manager.joystick_manager->bind_next_event_to(static_cast<Controller::Control>(item->id));
  }
  else if (item->id == MNID_AUTO_JOYSTICK_CFG)
  {
    //m_input_manager.use_game_controller(!item->toggled);
    m_input_manager.use_game_controller(!m_auto_joystick_cfg);
    m_input_manager.reset();
    recreate_menu();
  }
  else if(item->id == MNID_SCAN_JOYSTICKS)
  {
    m_input_manager.reset();
    recreate_menu();
  }
}

void
JoystickMenu::refresh_menu_item(Controller::Control id)
{
  ItemControlField* itemcf = dynamic_cast<ItemControlField*>(&get_item_by_id(static_cast<int>(id)));
  if (!itemcf) {
    return;
  }

  int button  = g_config->joystick_config.reversemap_joybutton(id);
  int axis    = g_config->joystick_config.reversemap_joyaxis(id);
  int hat_dir = g_config->joystick_config.reversemap_joyhat(id);

  if (button != -1)
  {
    itemcf->change_input(get_button_name(button));
  }
  else if (axis != 0)
  {
    std::ostringstream name;

    name << "Axis ";

    if (axis < 0)
      name << "-";
    else
      name << "+";

    if (abs(axis) == 1)
      name << "X";
    else if (abs(axis) == 2)
      name << "Y";
    else if (abs(axis) == 3)
      name << "X2";
    else if (abs(axis) == 4)
      name << "Y2";
    else
      name << abs(axis);

    itemcf->change_input(name.str());
  }
  else if (hat_dir != -1)
  {
    std::string name;

    switch (hat_dir)
    {
      case SDL_HAT_UP:
        name = "Hat Up";
        break;

      case SDL_HAT_DOWN:
        name = "Hat Down";
        break;

      case SDL_HAT_LEFT:
        name = "Hat Left";
        break;

      case SDL_HAT_RIGHT:
        name = "Hat Right";
        break;

      default:
        name = "Unknown hat_dir";
        break;
    }

    itemcf->change_input(name);
  }
  else
  {
    itemcf->change_input("None");
  }
}

void
JoystickMenu::refresh()
{
  if (m_joysticks_available)
  {
    refresh_menu_item(Controller::UP);
    refresh_menu_item(Controller::DOWN);
    refresh_menu_item(Controller::LEFT);
    refresh_menu_item(Controller::RIGHT);

    refresh_menu_item(Controller::JUMP);
    refresh_menu_item(Controller::ACTION);
    refresh_menu_item(Controller::START);
    refresh_menu_item(Controller::PEEK_LEFT);
    refresh_menu_item(Controller::PEEK_RIGHT);
    refresh_menu_item(Controller::PEEK_UP);
    refresh_menu_item(Controller::PEEK_DOWN);

    if (g_config->developer_mode) {
      refresh_menu_item(Controller::CONSOLE);
    }
    if (g_config->developer_mode) {
      refresh_menu_item(Controller::CHEAT_MENU);
    }
  }
}

// EOF
