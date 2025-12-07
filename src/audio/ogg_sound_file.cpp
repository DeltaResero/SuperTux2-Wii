// src/audio/ogg_sound_file.cpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#include "audio/ogg_sound_file.hpp"
#include "audio/sound_error.hpp"
#include "util/file_system.hpp"
#include <assert.h>
#include <bit>

OggSoundFile::OggSoundFile(const std::string& filename, double loop_begin_, double loop_at_) :
  file(nullptr),
  vorbis_file(),
  loop_begin(),
  loop_at()
{
  std::string path = FileSystem::find(filename);
  if (path.empty()) path = filename;

  file = fopen(path.c_str(), "rb");
  if (!file) {
      throw SoundError("Could not open Ogg file: " + path);
  }

  if (ov_open(file, &vorbis_file, NULL, 0) < 0) {
      fclose(file);
      throw SoundError("Input does not appear to be an Ogg bitstream: " + path);
  }

  vorbis_info* vi = ov_info(&vorbis_file, -1);

  channels        = vi->channels;
  rate            = vi->rate;
  bits_per_sample = 16;
  size            = static_cast<size_t> (ov_pcm_total(&vorbis_file, -1) * 2 * channels);

  double samples_begin = loop_begin_ * rate;
  double sample_loop   = loop_at_ * rate;

  this->loop_begin     = (ogg_int64_t) samples_begin;
  if(loop_begin_ < 0) {
    this->loop_at = (ogg_int64_t) -1;
  } else {
    this->loop_at = (ogg_int64_t) sample_loop;
  }
}

OggSoundFile::~OggSoundFile()
{
  ov_clear(&vorbis_file);
}

size_t
OggSoundFile::read(void* _buffer, size_t buffer_size)
{
  char*  buffer         = reinterpret_cast<char*> (_buffer);
  int    section        = 0;
  size_t totalBytesRead = 0;

  while(buffer_size>0) {
    int bigendian = (std::endian::native == std::endian::big) ? 1 : 0;

    size_t bytes_to_read    = buffer_size;
    if(loop_at > 0) {
      size_t      bytes_per_sample       = 2;
      ogg_int64_t time                   = ov_pcm_tell(&vorbis_file);
      ogg_int64_t samples_left_till_loop = loop_at - time;
      ogg_int64_t bytes_left_till_loop
        = samples_left_till_loop * bytes_per_sample;
      if(bytes_left_till_loop <= 4)
        break;

      if(bytes_left_till_loop < (ogg_int64_t) bytes_to_read) {
        bytes_to_read    = (size_t) bytes_left_till_loop;
      }
    }

    long bytesRead
      = ov_read(&vorbis_file, buffer, bytes_to_read, bigendian,
                2, 1, &section);
    if(bytesRead == 0) {
      break;
    }
    buffer_size    -= bytesRead;
    buffer         += bytesRead;
    totalBytesRead += bytesRead;
  }

  return totalBytesRead;
}

void
OggSoundFile::reset()
{
  ov_pcm_seek(&vorbis_file, loop_begin);
}

// EOF
