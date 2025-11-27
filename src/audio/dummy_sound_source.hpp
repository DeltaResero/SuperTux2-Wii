// src/audio/dummy_sound_source.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_AUDIO_DUMMY_SOUND_SOURCE_HPP
#define HEADER_SUPERTUX_AUDIO_DUMMY_SOUND_SOURCE_HPP

#include <memory>

class SoundSource;

std::unique_ptr<SoundSource> create_dummy_sound_source();

#endif // HEADER_SUPERTUX_AUDIO_DUMMY_SOUND_SOURCE_HPP

// EOF
