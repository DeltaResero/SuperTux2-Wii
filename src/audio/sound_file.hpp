// src/audio/sound_file.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_AUDIO_SOUND_FILE_HPP
#define HEADER_SUPERTUX_AUDIO_SOUND_FILE_HPP

#include <iostream>
#include <memory>

class SoundFile
{
public:
  SoundFile() :
    channels(),
    rate(),
    bits_per_sample(),
    size()
  {}

  virtual ~SoundFile()
  { }

  virtual size_t read(void* buffer, size_t buffer_size) = 0;
  virtual void reset() = 0;

  int channels;
  int rate;
  int bits_per_sample;
  /// size in bytes
  size_t size;

private:
  SoundFile(const SoundFile&) = delete;
  SoundFile& operator=(const SoundFile&) = delete;
};

std::unique_ptr<SoundFile> load_sound_file(const std::string& filename);

#endif // HEADER_SUPERTUX_AUDIO_SOUND_FILE_HPP

// EOF
