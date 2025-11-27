// src/audio/musicref.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2025 DeltaResero
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_AUDIO_MUSICREF_HPP
#define HEADER_SUPERTUX_AUDIO_MUSICREF_HPP

#include <config.h>

#ifdef USE_SDL_MIXER

#include "audio/sound_manager.hpp"

/** This class holds a reference to a music file and maintains a correct
 * refcount for that file.
 */
class MusicRef
{
public:
  MusicRef();
  MusicRef(const MusicRef& other);
  ~MusicRef();

  MusicRef& operator= (const MusicRef& other);

private:
  friend class SoundManager;
  MusicRef(SoundManager::MusicResource* music);

  SoundManager::MusicResource* music;
};

#endif // USE_SDL_MIXER

#endif // HEADER_SUPERTUX_AUDIO_MUSICREF_HPP

// EOF
