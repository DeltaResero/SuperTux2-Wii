//  SuperTux
//  Copyright (C) 2006 Matthias Braun <matze@braunis.de>
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

#include "gui/mousecursor.hpp"

#include <SDL.h>
#include <string>
#include <utility>

#include "supertux/gameconfig.hpp"
#include "supertux/globals.hpp"
#include "sprite/sprite.hpp"
#include "video/drawing_context.hpp"
#include "video/renderer.hpp"
#include "video/video_system.hpp"
#include "video/viewport.hpp"

MouseCursor* MouseCursor::current_ = nullptr;

MouseCursor::MouseCursor(SpritePtr sprite) :
  m_state(MouseCursorState::NORMAL),
  m_applied_state(MouseCursorState::HIDE),
  m_sprite(std::move(sprite)),
  m_reported_x(-1),
  m_reported_y(-1),
  m_pointer(0.0f, 0.0f),
  m_window_size(0, 0),
  m_fullscreen(false),
  m_lost(false)
#ifdef ENABLE_TOUCHSCREEN_SUPPORT
  , m_x(),
  m_y(),
  m_mobile_mode(false)
#endif
{
}

bool
MouseCursor::find_pointer(int& x, int& y)
{
  const Size window_size = VideoSystem::current()->get_window_size();
  if (x != m_reported_x || y != m_reported_y || m_window_size == Size(0, 0))
  {
    // SDL only reports anything new once the pointer moves
    m_reported_x = x;
    m_reported_y = y;
    m_pointer = Vector(static_cast<float>(x), static_cast<float>(y));
    m_lost = false;
  }
  else if (window_size != m_window_size || g_config->use_fullscreen != m_fullscreen)
  {
    // SDL keeps the old size's pixels until the pointer moves, so a screen filled before and after is rescaled here
    if (m_fullscreen && g_config->use_fullscreen)
    {
      m_pointer.x *= static_cast<float>(window_size.width) / static_cast<float>(m_window_size.width);
      m_pointer.y *= static_cast<float>(window_size.height) / static_cast<float>(m_window_size.height);
    }
    else
    {
      // A window that moved or changed size leaves no way to tell where the pointer is now
      m_lost = true;
    }
  }
  m_window_size = window_size;
  m_fullscreen = g_config->use_fullscreen;

  x = static_cast<int>(m_pointer.x);
  y = static_cast<int>(m_pointer.y);
  return !m_lost;
}

void
MouseCursor::set_state(MouseCursorState state)
{
  m_state = state;
}

void
MouseCursor::apply_state(MouseCursorState state)
{
  if (m_applied_state != state)
  {
    m_applied_state = state;

    switch(state)
    {
      case MouseCursorState::NORMAL:
        m_sprite->set_action("normal");
        break;

      case MouseCursorState::CLICK:
        m_sprite->set_action("click");
        break;

      case MouseCursorState::LINK:
        m_sprite->set_action("link");
        break;

      case MouseCursorState::HIDE:
        break;
    }
  }
}

void
MouseCursor::draw(DrawingContext& context)
{
  if (!g_config->custom_mouse_cursor) return;
  if (m_state != MouseCursorState::HIDE)
  {
    int x, y;
    Uint32 ispressed = SDL_GetMouseState(&x, &y);
    bool known = find_pointer(x, y);

#ifdef ENABLE_TOUCHSCREEN_SUPPORT
    if (m_mobile_mode)
    {
      x = m_x;
      y = m_y;
      known = true;
    }
#endif

    if (!known)
    {
      return;
    }

    if (ispressed & SDL_BUTTON(1) || ispressed & SDL_BUTTON(2))
    {
      apply_state(MouseCursorState::CLICK);
    }
    else
    {
      apply_state(m_state);
    }

    Vector mouse_pos = VideoSystem::current()->get_viewport().to_ui(x, y);

    m_sprite->draw(context.color(), mouse_pos, LAYER_GUI + 100);
  }
}

/* EOF */
