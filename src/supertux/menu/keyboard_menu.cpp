// src/supertux/menu/keyboard_menu.cpp
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

#include "supertux/menu/keyboard_menu.hpp"

#include "control/keyboard_manager.hpp"
#include "gui/item_controlfield.hpp"
#include "supertux/gameconfig.hpp"
#include "supertux/globals.hpp"
#include "util/gettext.hpp"

KeyboardMenu::KeyboardMenu(InputManager& input_manager) :
  m_input_manager(input_manager)
{
  add_label("Setup Keyboard");
  add_hl();
  add_controlfield(Controller::UP,         "Up");
  add_controlfield(Controller::DOWN,       "Down");
  add_controlfield(Controller::LEFT,       "Left");
  add_controlfield(Controller::RIGHT,      "Right");
  add_controlfield(Controller::JUMP,       "Jump");
  add_controlfield(Controller::ACTION,     "Action");
  add_controlfield(Controller::PEEK_LEFT,  "Peek Left");
  add_controlfield(Controller::PEEK_RIGHT, "Peek Right");
  add_controlfield(Controller::PEEK_UP,    "Peek Up");
  add_controlfield(Controller::PEEK_DOWN,  "Peek Down");
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
  add_toggle(Controller::CONTROLCOUNT, "Jump with Up", &g_config->keyboard_config.jump_with_up_kbd);
  add_hl();
  add_back("Back");
  refresh();
}

KeyboardMenu::~KeyboardMenu()
{}

std::string
KeyboardMenu::get_key_name(SDL_Keycode key) const
{
  switch(key) {
    case SDLK_UNKNOWN:
      return "None";
    case SDLK_UP:
      return "Up cursor";
    case SDLK_DOWN:
      return "Down cursor";
    case SDLK_LEFT:
      return "Left cursor";
    case SDLK_RIGHT:
      return "Right cursor";
    case SDLK_RETURN:
      return "Return";
    case SDLK_SPACE:
      return "Space";
    case SDLK_RSHIFT:
      return "Right Shift";
    case SDLK_LSHIFT:
      return "Left Shift";
    case SDLK_RCTRL:
      return "Right Control";
    case SDLK_LCTRL:
      return "Left Control";
    case SDLK_RALT:
      return "Right Alt";
    case SDLK_LALT:
      return "Left Alt";
    case SDLK_RGUI:
      return "Right Command";
    case SDLK_LGUI:
      return "Left Command";
    default:
      return SDL_GetKeyName(static_cast<SDL_Keycode>(key));
  }
}

void
KeyboardMenu::menu_action(MenuItem* item)
{
  if(item->id >= 0 && item->id < Controller::CONTROLCOUNT){
    ItemControlField* itemcf = dynamic_cast<ItemControlField*>(item);
    if (!itemcf) {
      return;
    }
    itemcf->change_input("Press Key");
    m_input_manager.keyboard_manager->bind_next_event_to(static_cast<Controller::Control>(item->id));
  }
}

void
KeyboardMenu::refresh()
{
  KeyboardConfig& kbd_cfg = g_config->keyboard_config;
  ItemControlField* micf;

  micf = dynamic_cast<ItemControlField*>(&get_item_by_id((int) Controller::UP));
  if (micf) micf->change_input(get_key_name(kbd_cfg.reversemap_key(Controller::UP)));
  micf = dynamic_cast<ItemControlField*>(&get_item_by_id((int) Controller::DOWN));
  if (micf) micf->change_input(get_key_name(kbd_cfg.reversemap_key(Controller::DOWN)));
  micf = dynamic_cast<ItemControlField*>(&get_item_by_id((int) Controller::LEFT));
  if (micf) micf->change_input(get_key_name(kbd_cfg.reversemap_key(Controller::LEFT)));
  micf = dynamic_cast<ItemControlField*>(&get_item_by_id((int) Controller::RIGHT));
  if (micf) micf->change_input(get_key_name(kbd_cfg.reversemap_key(Controller::RIGHT)));
  micf = dynamic_cast<ItemControlField*>(&get_item_by_id((int) Controller::JUMP));
  if (micf) micf->change_input(get_key_name(kbd_cfg.reversemap_key(Controller::JUMP)));
  micf = dynamic_cast<ItemControlField*>(&get_item_by_id((int) Controller::ACTION));
  if (micf) micf->change_input(get_key_name(kbd_cfg.reversemap_key(Controller::ACTION)));
  micf = dynamic_cast<ItemControlField*>(&get_item_by_id((int) Controller::PEEK_LEFT));
  if (micf) micf->change_input(get_key_name(kbd_cfg.reversemap_key(Controller::PEEK_LEFT)));
  micf = dynamic_cast<ItemControlField*>(&get_item_by_id((int) Controller::PEEK_RIGHT));
  if (micf) micf->change_input(get_key_name(kbd_cfg.reversemap_key(Controller::PEEK_RIGHT)));
  micf = dynamic_cast<ItemControlField*>(&get_item_by_id((int) Controller::PEEK_UP));
  if (micf) micf->change_input(get_key_name(kbd_cfg.reversemap_key(Controller::PEEK_UP)));
  micf = dynamic_cast<ItemControlField*>(&get_item_by_id((int) Controller::PEEK_DOWN));
  if (micf) micf->change_input(get_key_name(kbd_cfg.reversemap_key(Controller::PEEK_DOWN)));

  if (g_config->developer_mode) {
    micf = dynamic_cast<ItemControlField*>(&get_item_by_id((int) Controller::CHEAT_MENU));
    if (micf) micf->change_input(get_key_name(kbd_cfg.reversemap_key(Controller::CHEAT_MENU)));
  }

  if (g_config->developer_mode) {
    micf = dynamic_cast<ItemControlField*>(&get_item_by_id((int) Controller::CONSOLE));
    if (micf) micf->change_input(get_key_name(kbd_cfg.reversemap_key(Controller::CONSOLE)));
  }
}

// EOF
