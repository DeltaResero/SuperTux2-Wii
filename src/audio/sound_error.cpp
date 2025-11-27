// src/audio/sound_error.cpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#include "audio/sound_error.hpp"

SoundError::SoundError(const std::string& message_) throw() :
  message(message_)
{
}

SoundError::~SoundError() throw()
{}

const char*
SoundError::what() const throw()
{
  return message.c_str();
}

// EOF
