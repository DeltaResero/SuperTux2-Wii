// src/audio/sound_error.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_AUDIO_SOUND_ERROR_HPP
#define HEADER_SUPERTUX_AUDIO_SOUND_ERROR_HPP

#include <stdexcept>
#include <string>

class SoundError : public std::exception
{
public:
  SoundError(const std::string& message) throw();
  virtual ~SoundError() throw();

  const char* what() const throw();
private:
  std::string message;
};

#endif // HEADER_SUPERTUX_AUDIO_SOUND_ERROR_HPP

// EOF
