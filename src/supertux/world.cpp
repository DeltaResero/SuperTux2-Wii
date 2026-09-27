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

#include "supertux/world.hpp"

#include "supertux/gameconfig.hpp"
#include "supertux/globals.hpp"
#include "util/file_system.hpp"
#include "util/log.hpp"
#include "util/reader_document.hpp"
#include "util/reader_mapping.hpp"

std::unique_ptr<World>
World::from_directory(const std::string& directory)
{
  std::unique_ptr<World> world(new World(directory));

  std::string info_filename = FileSystem::join(directory, "info");

  try
  {
    auto doc = ReaderDocument::from_file(info_filename);
    auto root = doc.get_root();

    if (root.get_name() != "supertux-world" &&
        root.get_name() != "supertux-level-subset")
    {
      throw std::runtime_error("File is not a world or levelsubset file");
    }

    auto info = root.get_mapping();

    info.get("title", world->m_title);
    info.get("description", world->m_description);
    info.get("levelset", world->m_is_levelset, true);
    info.get("hide-from-contribs", world->m_hide_from_contribs, false);

    return world;
  }
  catch (const std::exception& err)
  {
    log_warning << "Failed to load " << info_filename << ":" << err.what() << std::endl;

    world->m_hide_from_contribs = true;

    return world;
  }
}

World::World(const std::string& directory) :
  m_title(),
  m_description(),
  m_is_levelset(true),
  m_basedir(directory),
  m_hide_from_contribs(false)
{
}

std::string
World::get_worldmap_filename() const
{
  return FileSystem::join(m_basedir, "worldmap.stwm");
}

std::string
World::get_savegame_filename() const
{
  const std::string worlddirname = FileSystem::basename(m_basedir);
  std::ostringstream stream;
  stream << "profile" << g_config->profile << "/" << worlddirname << ".stsg";
  return stream.str();
}

/* EOF */
