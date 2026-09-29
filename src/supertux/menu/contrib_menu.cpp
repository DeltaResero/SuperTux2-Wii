//  SuperTux
//  Copyright (C) 2009 Ingo Ruhnke <grumbel@gmail.com>
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

#include "supertux/menu/contrib_menu.hpp"

#include <sstream>

#include "gui/item_action.hpp"
#include "gui/menu_item.hpp"
#include "gui/menu_manager.hpp"
#include "supertux/game_manager.hpp"
#include "supertux/levelset.hpp"
#include "supertux/menu/contrib_levelset_menu.hpp"
#include "supertux/player_status.hpp"
#include "supertux/world.hpp"
#include "util/file_system.hpp"
#include "util/log.hpp"

ContribMenu::ContribMenu() :
  m_contrib_worlds()
{
  // Generating contrib levels list by making use of Level Subset
  std::vector<std::string> level_worlds;

  for (const std::string& filename : FileSystem::enumerate("levels"))
  {
    std::string filepath = FileSystem::join("levels", filename);
    if (FileSystem::is_directory(FileSystem::find(filepath)))
    {
      level_worlds.push_back(filepath);
    }
  }

  add_label("Contrib Levels");
  add_hl();

  int i = 0;
  for (std::vector<std::string>::const_iterator it = level_worlds.begin(); it != level_worlds.end(); ++it)
  {
    try
    {
      auto levelset =
        std::unique_ptr<Levelset>(new Levelset(*it, /* recursively = */ true));
      if (levelset->get_num_levels() == 0)
        continue;

      std::unique_ptr<World> world = World::from_directory(*it);
      if (!world->hide_from_contribs())
      {
        if (world->is_levelset())
        {
          std::ostringstream title;
          title << "[" << world->get_title() << "]";
          std::ostringstream desc;
          desc << world->get_description();
          add_entry(i++, title.str()).set_help(desc.str());
          m_contrib_worlds.push_back(std::move(world));
        }
        else if (world->is_worldmap())
        {
          std::ostringstream title;
          title << world->get_title();
          std::ostringstream desc;
          desc << world->get_description();
          add_entry(i++, title.str()).set_help(desc.str());
          m_contrib_worlds.push_back(std::move(world));
        }
        else
        {
          log_warning << "unknown World type" << std::endl;
        }
      }
    }
    catch(std::exception& e)
    {
      log_info << "Couldn't parse levelset info for '" << *it << "': " << e.what() << std::endl;
    }
  }

  add_hl();
  add_back("Back");
}

void
ContribMenu::menu_action(MenuItem& item)
{
  int index = item.get_id();
  if (index != -1)
  {
    // reload the World so that we have something that we can safely
    // std::move() around without wreaking the ContribMenu
    std::unique_ptr<World> world = World::from_directory(m_contrib_worlds[index]->get_basedir());
    if (!world->is_levelset())
    {
      GameManager::current()->start_worldmap(*world);
    }
    else
    {
      MenuManager::instance().push_menu(std::unique_ptr<Menu>(new ContribLevelsetMenu(std::move(world))));
    }
  }
}

/* EOF */
