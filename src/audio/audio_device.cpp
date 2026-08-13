// src/audio/audio_device.cpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
//  SuperTux
//  Copyright (C) 2026 DeltaResero
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

#include "audio/audio_device.hpp"

#include <config.h>

#include <stdexcept>

AudioBackend audio_backend_from_string(const std::string& name)
{
  if (name == "auto")
    return AudioBackend::Automatic;

#ifdef ENABLE_OPENAL
  if (name == "openal")
    return AudioBackend::OpenAL;
#endif

#ifdef ENABLE_SDL_MIXER
  if (name == "sdl" || name == "sdl_mixer")
    return AudioBackend::SdlMixer;
#endif

  /* Naming a backend this build does not carry is worth saying out loud
     rather than quietly falling back, since the whole point of asking is to
     hear that one. */
  throw std::runtime_error("Unknown or unavailable audio backend: " + name);
}

/* EOF */
