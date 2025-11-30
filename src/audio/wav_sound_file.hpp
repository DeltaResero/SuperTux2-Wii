// src/audio/wav_sound_file.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_AUDIO_WAV_SOUND_FILE_HPP
#define HEADER_SUPERTUX_AUDIO_WAV_SOUND_FILE_HPP

#include <fstream>
#include <string>
#include "audio/sound_file.hpp"

class WavSoundFile : public SoundFile
{
public:
  WavSoundFile(const std::string& filename);
  ~WavSoundFile();

  size_t read(void* buffer, size_t buffer_size);
  void reset();

private:
  std::ifstream file;
  std::streampos datastart;

private:
  WavSoundFile(const WavSoundFile&);
  WavSoundFile& operator=(const WavSoundFile&);
};

#endif // HEADER_SUPERTUX_AUDIO_WAV_SOUND_FILE_HPP

// EOF
