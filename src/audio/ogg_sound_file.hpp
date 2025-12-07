// src/audio/ogg_sound_file.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_AUDIO_OGG_SOUND_FILE_HPP
#define HEADER_SUPERTUX_AUDIO_OGG_SOUND_FILE_HPP

#include <string>
#include <vorbis/vorbisfile.h>
#include <stdio.h>

#include "audio/sound_file.hpp"

class OggSoundFile : public SoundFile
{
public:
  OggSoundFile(const std::string& filename, double loop_begin, double loop_at);
  ~OggSoundFile();

  size_t read(void* buffer, size_t buffer_size);
  void reset();

private:
  FILE*          file;
  OggVorbis_File vorbis_file;
  ogg_int64_t    loop_begin;
  ogg_int64_t    loop_at;

private:
  OggSoundFile(const OggSoundFile&);
  OggSoundFile& operator=(const OggSoundFile&);
};


#endif // HEADER_SUPERTUX_AUDIO_OGG_SOUND_FILE_HPP

// EOF
