// src/sprite/sprite_manager.cpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
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

#include "sprite/sprite_manager.hpp"

#include <algorithm>

#include "sprite/sprite.hpp"
#include "util/file_system.hpp"
#include "util/reader_document.hpp"
#include "util/reader_mapping.hpp"

#include <sstream>
#include <stdexcept>
#include <exception>
#include <utility>


SpriteManager::SpriteManager() :
  sprites(),
  held()
{
}

SpriteManager::~SpriteManager()
{
}

SpritePtr
SpriteManager::create(const std::string& name)
{
  Sprites::iterator i = sprites.find(name);
  std::shared_ptr<SpriteData> data;
  if(i != sprites.end())
    data = i->second.lock();

  if(!data) {
    // try loading the spritefile
    data = load(name);
    if(!data) {
      std::stringstream msg;
      msg << "Sprite '" << name << "' not found.";
      throw std::runtime_error(msg.str());
    }
    sprites[name] = data;
    held.push_back(data);
  }

  return SpritePtr(new Sprite(data));
}

void
SpriteManager::release_unused()
{
  /* Let go first, so that anything whose only remaining claim was this list
     goes now and hands its pictures back. */
  held.clear();

  std::erase_if(sprites, [](const Sprites::value_type& entry) {
    return entry.second.expired();
  });

  /* Then take hold again of whatever is still in play, so the next scene
     starts from the same footing this one did. */
  for(const auto& entry : sprites) {
    if(auto data = entry.second.lock())
      held.push_back(std::move(data));
  }
}

std::shared_ptr<SpriteData>
SpriteManager::load(const std::string& filename)
{
  ReaderDocument doc;

  try {
    if(filename.size() >= 7 && filename.compare(filename.size() - 7, 7, ".sprite") == 0) {
      // Sprite file
      doc = ReaderDocument::parse(filename);
    } else {
      // Load image file directly
      std::stringstream lisptext;
      lisptext << "(supertux-sprite (action "
               <<    "(name \"default\") "
               <<    "(images \"" << FileSystem::basename(filename) << "\")))";
      doc = ReaderDocument::parse(lisptext);
    }
  } catch(const std::exception& e) {
    std::ostringstream msg;
    msg << "Parse error when trying to load sprite '" << filename
        << "': " << e.what() << "\n";
    throw std::runtime_error(msg.str());
  }

  auto root = doc.get_root();

  if(root.get_name() != "supertux-sprite") {
    std::ostringstream msg;
    msg << "'" << filename << "' is not a supertux-sprite file";
    throw std::runtime_error(msg.str());
  } else {
    return std::make_shared<SpriteData>(root.get_mapping(),
                                        FileSystem::dirname(filename));
  }
}

/* EOF */
