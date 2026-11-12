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

#include "sprite/sprite.hpp"
#include "util/file_system.hpp"
#include "util/log.hpp"
#include "util/reader_document.hpp"
#include "util/reader_mapping.hpp"
#include "util/string_util.hpp"

#include <exception>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <utility>

namespace {

// An image name loads as a one-frame sprite
const std::string DUMMY_SPRITE = "images/engine/missing.png";

} // namespace

SpriteManager::SpriteManager() :
  sprites(),
  held()
{
}

SpritePtr
SpriteManager::create(const std::string& name)
{
  Sprites::iterator i = sprites.find(name);
  std::shared_ptr<SpriteData> data;
  if (i != sprites.end())
    data = i->second.lock();

  if (!data) {
    // try loading the spritefile
    try {
      data = load(name);
    } catch (const std::exception& e) {
      if (name == DUMMY_SPRITE)
        throw;
      log_warning << "Couldn't load sprite '" << name << "' (now using dummy sprite): " << e.what() << std::endl;
      return create(DUMMY_SPRITE);
    }
    if (data == nullptr) {
      std::stringstream msg;
      msg << "Sprite '" << name << "' not found.";
      throw std::runtime_error(msg.str());
    }
    sprites[name] = data;
    held.push_back(data);
  }

  return std::make_unique<Sprite>(std::move(data));
}

void
SpriteManager::release_unused()
{
  // Whatever only this list was keeping goes now
  held.clear();

  std::erase_if(sprites, [](const Sprites::value_type& entry) {
    return entry.second.expired();
  });

  // The rest is still in play and is held again for the next screen
  for (const auto& entry : sprites) {
    if (auto data = entry.second.lock())
      held.push_back(std::move(data));
  }
}

std::shared_ptr<SpriteData>
SpriteManager::load(const std::string& filename)
{
  ReaderDocument doc = [filename](){
    try {
      if (StringUtil::has_suffix(filename, ".sprite")) {
        return ReaderDocument::from_file(filename);
      } else {
        std::stringstream text;
        text << "(supertux-sprite (action "
             << "(name \"default\") "
             << "(images \"" << FileSystem::basename(filename) << "\")))";
        return ReaderDocument::from_stream(text, filename);
      }
    } catch(const std::exception& e) {
      std::ostringstream msg;
      msg << "Parse error when trying to load sprite '" << filename
      << "': " << e.what() << "\n";
      throw std::runtime_error(msg.str());
    }
  }();

  auto root = doc.get_root();

  if (root.get_name() != "supertux-sprite") {
    std::ostringstream msg;
    msg << "'" << filename << "' is not a supertux-sprite file";
    throw std::runtime_error(msg.str());
  } else {
    return std::make_shared<SpriteData>(root.get_mapping());
  }
}

/* EOF */
