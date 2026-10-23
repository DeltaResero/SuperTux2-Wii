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

#include "control/keyboard_config.hpp"

#include <optional>
#include <string>
#include <utility>

#include "util/log.hpp"
#include "util/reader_mapping.hpp"
#include "util/writer.hpp"

KeyboardConfig::KeyboardConfig() :
  m_keymap(),
  m_configurable_controls(),
  m_alt_action_key(SDLK_LCTRL),
  m_jump_with_up_kbd(false)
{
  // initialize default keyboard map
  m_keymap[SDLK_LEFT]      = Control::LEFT;
  m_keymap[SDLK_RIGHT]     = Control::RIGHT;
  m_keymap[SDLK_UP]        = Control::UP;
  m_keymap[SDLK_DOWN]      = Control::DOWN;
  m_keymap[SDLK_SPACE]     = Control::JUMP;
  m_keymap[SDLK_LSHIFT]    = Control::ACTION;
  m_keymap[SDLK_LCTRL]     = Control::ACTION;
  m_keymap[SDLK_ESCAPE]    = Control::ESCAPE;
  m_keymap[SDLK_p]         = Control::START;
  m_keymap[SDLK_PAUSE]     = Control::START;
  m_keymap[SDLK_RETURN]    = Control::MENU_SELECT;
  m_keymap[SDLK_KP_ENTER]  = Control::MENU_SELECT;
  m_keymap[SDLK_BACKQUOTE] = Control::CONSOLE;
  m_keymap[SDLK_DELETE]    = Control::PEEK_LEFT;
  m_keymap[SDLK_PAGEDOWN]  = Control::PEEK_RIGHT;
  m_keymap[SDLK_HOME]      = Control::PEEK_UP;
  m_keymap[SDLK_END]       = Control::PEEK_DOWN;
  m_keymap[SDLK_F1]        = Control::CHEAT_MENU;
  m_keymap[SDLK_F2]        = Control::DEBUG_MENU;
  m_keymap[SDLK_BACKSPACE] = Control::REMOVE;

  m_configurable_controls = {
    Control::UP,
    Control::DOWN,
    Control::LEFT,
    Control::RIGHT,
    Control::JUMP,
    Control::ACTION,
    Control::PEEK_LEFT,
    Control::PEEK_RIGHT,
    Control::PEEK_UP,
    Control::PEEK_DOWN,
    Control::CONSOLE,
    Control::CHEAT_MENU,
    Control::DEBUG_MENU
  };
}

void
KeyboardConfig::read(const ReaderMapping& keymap_mapping)
{
  // backwards compatibility:
  // keycode values changed between SDL1 and SDL2, so we skip old SDL1
  // based values and use the defaults instead on the first read of
  // the config file
  bool config_is_sdl2 = false;
  keymap_mapping.get("sdl2", config_is_sdl2);
  if (!config_is_sdl2)
    return;

  keymap_mapping.get("jump-with-up", m_jump_with_up_kbd);

  bool action_read = false;
  bool alt_action_read = false;
  auto iter = keymap_mapping.get_iter();
  while (iter.next())
  {
    if (iter.get_key() != "map")
      continue;
    int key = -1;
    auto map = iter.as_mapping();
    map.get("key", key);

    std::string control_text;
    map.get("control", control_text);

    if (control_text == "action-alt") {
      bind_alt_action_key(static_cast<SDL_Keycode>(key));
      alt_action_read = true;
      continue;
    }

    const std::optional<Control> maybe_control = Control_from_string(control_text);
    if (maybe_control) {
      if (m_configurable_controls.count(*maybe_control)) {
        // older configs saved both Action keys as "action", so the second becomes the alternate
        if (*maybe_control == Control::ACTION && action_read) {
          bind_alt_action_key(static_cast<SDL_Keycode>(key));
          alt_action_read = true;
        } else {
          bind_key(static_cast<SDL_Keycode>(key), *maybe_control);
          action_read = action_read || *maybe_control == Control::ACTION;
        }
      }
    } else {
      log_warning << "Invalid control '" << control_text << "' in keymap" << std::endl;
    }
  }

  // a saved Action with no alternate means the player had none
  if (action_read && !alt_action_read && m_alt_action_key != SDLK_UNKNOWN) {
    m_keymap.erase(m_alt_action_key);
    m_alt_action_key = SDLK_UNKNOWN;
  }
}

void
KeyboardConfig::bind_key(SDL_Keycode key, Control control)
{
  // remove all previous mappings for that control and for that key, but leave the alternate Action key
  for (auto i = m_keymap.begin(); i != m_keymap.end(); /* no ++i */)
  {
    if (i->second == control && i->first != m_alt_action_key)
    {
      auto e = i;
      ++i;
      m_keymap.erase(e);
    }
    else
    {
      ++i;
    }
  }

  if (key == m_alt_action_key)
    m_alt_action_key = SDLK_UNKNOWN;

  auto i = m_keymap.find(key);
  if (i != m_keymap.end())
    m_keymap.erase(i);

  // add new mapping
  m_keymap[key] = control;
}

void
KeyboardConfig::bind_alt_action_key(SDL_Keycode key)
{
  if (m_alt_action_key != SDLK_UNKNOWN)
    m_keymap.erase(m_alt_action_key);

  m_keymap[key] = Control::ACTION;
  m_alt_action_key = key;
}

SDL_Keycode
KeyboardConfig::reversemap_key(Control c) const
{
  for (const auto& i : m_keymap)
  {
    if (i.second == c && i.first != m_alt_action_key)
    {
      return i.first;
    }
  }

  return SDLK_UNKNOWN;
}

void
KeyboardConfig::write(Writer& writer)
{
  // this flag handles the transition from SDL1 to SDL2, as keycodes
  // are incompatible between the two, if it's not set an old SDL1
  // config file is assumed and controls are reset to default
  writer.write("sdl2", true);

  writer.write("jump-with-up", m_jump_with_up_kbd);

  for (const auto& i : m_keymap)
  {
    writer.start_list("map");
    writer.write("key", static_cast<int>(i.first));
    writer.write("control", i.first == m_alt_action_key ? "action-alt" : Control_to_string(i.second));
    writer.end_list("map");
  }
}

/* EOF */
