//  SuperTux
//  Copyright (C) 2026 DeltaResero
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

#ifndef HEADER_SUPERTUX_SUPERTUX_MENU_RENDERER_MENU_HPP
#define HEADER_SUPERTUX_SUPERTUX_MENU_RENDERER_MENU_HPP

#include <string>
#include <vector>

#include "gui/menu.hpp"
#include "video/video_system.hpp"

/** Picks the renderer the game starts with next time */
class RendererMenu final : public Menu
{
public:
  RendererMenu();

  void menu_action(MenuItem& item) override;

private:
  std::vector<VideoSystem::Info> m_video_systems;

private:
  RendererMenu(const RendererMenu&) = delete;
  RendererMenu& operator=(const RendererMenu&) = delete;
};

/** Names the renderer the config asks for, and the one in use when that differs */
std::string current_renderer_text();

#endif

/* EOF */
