// src/audio/sound_file.cpp
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

/** Used SDL_mixer and glest source as reference */

#include "audio/sound_file.hpp"

#include <config.h>

#include <cstring>
#include <stdint.h>
#include <fstream>
#include <sstream>

#include "audio/sound_error.hpp"
#include "audio/ogg_sound_file.hpp"
#include "audio/wav_sound_file.hpp"
#include "util/reader_document.hpp"
#include "util/reader_mapping.hpp"
#include "util/file_system.hpp"
#include "util/log.hpp"

MusicReference load_music_reference(const std::string& filename)
{
  auto doc = ReaderDocument::parse(filename);
  auto root = doc.get_root();
  if(root.get_name() != "supertux-music")
  {
    throw SoundError("file is not a supertux-music file.");
  }

  auto music = root.get_mapping();

  MusicReference reference;
  reference.loop_begin = 0;
  reference.loop_at    = -1;

  music.get("file", reference.file);
  music.get("loop-begin", reference.loop_begin);
  music.get("loop-at", reference.loop_at);

  if(reference.loop_begin < 0) {
    throw SoundError("can't loop from negative value");
  }

  std::string basedir = FileSystem::dirname(filename);
  reference.file = FileSystem::normalize(basedir + reference.file);

  return reference;
}

std::unique_ptr<SoundFile> load_music_file(const std::string& filename)
{
  const MusicReference reference = load_music_reference(filename);
  return std::unique_ptr<SoundFile>(
    new OggSoundFile(reference.file, reference.loop_begin, reference.loop_at));
}

std::unique_ptr<SoundFile> load_sound_file(const std::string& filename)
{
  if(filename.length() > 6
     && filename.compare(filename.length() - 6, 6, ".music") == 0) {
    return load_music_file(filename);
  }

  const std::string path = FileSystem::find(filename);
  if(path.empty()) {
    throw SoundError("Couldn't open '" + filename + "': not found, using dummy sound file.");
  }

  try {
    char magic[4];
    {
      std::ifstream in(path, std::ios::in | std::ios::binary);
      if(!in.is_open())
        throw SoundError("Couldn't open '" + path + "'");
      if(!in.read(magic, sizeof(magic)))
        throw SoundError("Couldn't read magic, file too short");
    }

    if(strncmp(magic, "RIFF", 4) == 0)
      return std::unique_ptr<SoundFile>(new WavSoundFile(filename));
    else if(strncmp(magic, "OggS", 4) == 0)
      return std::unique_ptr<SoundFile>(new OggSoundFile(filename, 0, -1));
    else
      throw SoundError("Unknown file format");
  } catch(std::exception& e) {
    std::stringstream msg;
    msg << "Couldn't read '" << filename << "': " << e.what();
    throw SoundError(msg.str());
  }
}

/* EOF */
