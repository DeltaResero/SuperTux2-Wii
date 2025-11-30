// src/audio/wav_sound_file.cpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#include "audio/wav_sound_file.hpp"
#include "audio/sound_error.hpp"
#include "util/file_system.hpp"
#include "util/log.hpp"
#include <cstring>
#include <algorithm>
#include <bit>
#include <SDL.h>

static inline uint32_t read32LE(std::ifstream& file)
{
  uint32_t result;
  file.read(reinterpret_cast<char*>(&result), 4);
  if(!file) throw SoundError("file too short");

  if constexpr (std::endian::native == std::endian::big) {
    return std::byteswap(result);
  } else {
    return result;
  }
}

static inline uint16_t read16LE(std::ifstream& file)
{
  uint16_t result;
  file.read(reinterpret_cast<char*>(&result), 2);
  if(!file) throw SoundError("file too short");

  if constexpr (std::endian::native == std::endian::big) {
    return std::byteswap(result);
  } else {
    return result;
  }
}

WavSoundFile::WavSoundFile(const std::string& filename) :
  file(),
  datastart(0)
{
  std::string path = FileSystem::find(filename);
  file.open(path, std::ios::binary);

  if(!file.is_open())
      throw SoundError("Could not open WAV file: " + filename);

  char magic[4];
  file.read(magic, 4);
  if(strncmp(magic, "RIFF", 4) != 0) {
    throw SoundError("file is not a RIFF wav file");
  }

  uint32_t wavelen = read32LE(file);
  (void) wavelen;

  file.read(magic, 4);
  if(strncmp(magic, "WAVE", 4) != 0)
    throw SoundError("file is not a valid RIFF/WAVE file");

  char chunkmagic[4];
  uint32_t chunklen;

  // search audio data format chunk
  do {
    file.read(chunkmagic, 4);
    if(!file)
      throw SoundError("EOF while searching format chunk");
    chunklen = read32LE(file);

    if(strncmp(chunkmagic, "fmt ", 4) == 0)
      break;

    if(strncmp(chunkmagic, "fact", 4) == 0
       || strncmp(chunkmagic, "LIST", 4) == 0) {
      // skip chunk
      file.seekg(chunklen, std::ios::cur);
      if(!file)
        throw SoundError("EOF while searching fmt chunk");
    } else {
      throw SoundError("complex WAVE files not supported");
    }
  } while(true);

  if(chunklen < 16)
    throw SoundError("Format chunk too short");

  // parse format
  uint16_t encoding = read16LE(file);
  if(encoding != 1)
    throw SoundError("only PCM encoding supported");
  channels = read16LE(file);
  rate = read32LE(file);
  uint32_t byterate = read32LE(file);
  (void) byterate;
  uint16_t blockalign = read16LE(file);
  (void) blockalign;
  bits_per_sample = read16LE(file);

  if(chunklen > 16) {
    file.seekg(chunklen - 16, std::ios::cur);
    if(!file)
      throw SoundError("EOF while reading rest of format chunk");
  }

  // set file offset to DATA chunk data
  do {
    file.read(chunkmagic, 4);
    if(!file)
      throw SoundError("EOF while searching data chunk");
    chunklen = read32LE(file);

    if(strncmp(chunkmagic, "data", 4) == 0)
      break;

    // skip chunk
    file.seekg(chunklen, std::ios::cur);
    if(!file)
      throw SoundError("EOF while searching fmt chunk");
  } while(true);

  datastart = file.tellg();
  size = static_cast<size_t> (chunklen);
}

WavSoundFile::~WavSoundFile()
{
  if(file.is_open()) file.close();
}

void
WavSoundFile::reset()
{
  file.clear();
  file.seekg(datastart);
}

size_t
WavSoundFile::read(void* buffer, size_t buffer_size)
{
  std::streampos end = datastart + static_cast<std::streamoff>(size);
  std::streampos cur = file.tellg();
  if(cur >= end)
    return 0;

  size_t readsize = std::min(static_cast<size_t> (end - cur), buffer_size);
  file.read(reinterpret_cast<char*>(buffer), readsize);
  if(file.gcount() != static_cast<std::streamsize>(readsize))
    throw SoundError("read error while reading samples");

  if constexpr (std::endian::native == std::endian::big) {
    if (bits_per_sample == 16) {
      // Modern C++23 byteswap
      uint16_t* samples = static_cast<uint16_t*>(buffer);
      size_t count = readsize / 2;
      for (size_t i = 0; i < count; ++i) {
        samples[i] = std::byteswap(samples[i]);
      }
    }
  }

  return readsize;
}

// EOF
