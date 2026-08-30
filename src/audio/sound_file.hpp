// src/audio/sound_file.hpp
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

#ifndef HEADER_SUPERTUX_AUDIO_SOUND_FILE_HPP
#define HEADER_SUPERTUX_AUDIO_SOUND_FILE_HPP

#include <iostream>
#include <memory>
#include <string>

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

/** What a .music wrapper says: which audio file to play and where it loops.
    Kept apart from reading the audio itself because a device that hands the
    file to a library rather than decoding it wants these figures and not the
    samples. */
struct MusicReference
{
  /** Path to the audio, already made relative to the search path. */
  std::string file{};
  /** Second to jump back to when the track reaches loop_at. */
  float loop_begin = 0.0f;
  /** Second at which to jump back, or negative for the end of the track. */
  float loop_at = -1.0f;
};

/** Read a .music wrapper. Throws if the file is not one. */
MusicReference load_music_reference(const std::string& filename);

std::unique_ptr<SoundFile> load_sound_file(const std::string& filename);

#endif

/* EOF */
