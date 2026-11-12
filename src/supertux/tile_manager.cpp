//  SuperTux
//  Copyright (C) 2004 Tobias Glaesser <tobi.web@gmx.de>
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

#include "supertux/tile_manager.hpp"

#include <map>
#include <utility>

#include "supertux/tile.hpp"
#include "supertux/tile_set.hpp"

TileManager::TileManager() :
  m_tilesets(),
  m_held()
{
}

std::shared_ptr<TileSet>
TileManager::get_tileset(const std::string &filename)
{
  auto it = m_tilesets.find(filename);
  if (it != m_tilesets.end())
  {
    if (auto tileset = it->second.lock())
      return tileset;
  }

  std::shared_ptr<TileSet> tileset = TileSet::from_file(filename);
  m_tilesets[filename] = tileset;
  m_held.push_back(tileset);
  return tileset;
}

void
TileManager::release_unused()
{
  // Whatever only this list was keeping goes now
  m_held.clear();

  std::erase_if(m_tilesets, [](const auto& entry) {
    return entry.second.expired();
  });

  // The rest is still drawn from and is held again for the next screen
  for (const auto& entry : m_tilesets) {
    if (auto tileset = entry.second.lock())
      m_held.push_back(std::move(tileset));
  }
}

/* EOF */
