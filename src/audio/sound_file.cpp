// src/audio/sound_file.cpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

/** Used SDL_mixer and glest source as reference */

#include "audio/sound_file.hpp"

#include <config.h>

#include <stdint.h>
#include <sstream>
#include <fstream>
#include <cstring>

#include "audio/sound_error.hpp"
#include "audio/ogg_sound_file.hpp"
#include "audio/wav_sound_file.hpp"
#include "util/reader_document.hpp"
#include "util/reader_mapping.hpp"
#include "util/file_system.hpp"
#include "util/log.hpp"

std::unique_ptr<SoundFile> load_music_file(const std::string& filename)
{
  auto doc = ReaderDocument::parse(filename);
  auto root = doc.get_root();
  if(root.get_name() != "supertux-music")
  {
    throw SoundError("file is not a supertux-music file.");
  }
  else
  {
    auto music = root.get_mapping();

    std::string raw_music_file;
    float loop_begin = 0;
    float loop_at    = -1;

    music.get("file", raw_music_file);
    music.get("loop-begin", loop_begin);
    music.get("loop-at", loop_at);

    if(loop_begin < 0) {
      throw SoundError("can't loop from negative value");
    }

    std::string basedir = FileSystem::dirname(filename);
    raw_music_file = FileSystem::normalize(FileSystem::join(basedir, raw_music_file));

    return std::unique_ptr<SoundFile>(new OggSoundFile(raw_music_file, loop_begin, loop_at));
  }
}

std::unique_ptr<SoundFile> load_sound_file(const std::string& filename)
{
  if(filename.length() > 6
     && filename.compare(filename.length() - 6, 6, ".music") == 0) {
    return load_music_file(filename);
  }

  std::string path = FileSystem::find(filename);
  if(path.empty()) {
      throw SoundError("Couldn't find file: " + filename);
  }

  std::ifstream file(path, std::ios::binary);
  if(!file) {
    throw SoundError("Couldn't open '" + path + "'");
  }

  try {
    char magic[4];
    file.read(magic, 4);
    if(!file)
      throw SoundError("Couldn't read magic, file too short");

    file.close(); // Close, let specific loaders reopen

    if(strncmp(magic, "RIFF", 4) == 0)
      return std::unique_ptr<SoundFile>(new WavSoundFile(path));
    else if(strncmp(magic, "OggS", 4) == 0)
      return std::unique_ptr<SoundFile>(new OggSoundFile(path, 0, -1));
    else
      throw SoundError("Unknown file format");
  } catch(std::exception& e) {
    throw SoundError(std::string("Couldn't read '") + filename + "': " + e.what());
  }
}

// EOF
