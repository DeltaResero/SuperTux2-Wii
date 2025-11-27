// src/scripting/level_time.cpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#include "object/level_time.hpp"
#include "scripting/level_time.hpp"

#define NOIMPL      log_fatal << __PRETTY_FUNCTION__ << " not implemented."

namespace scripting {

LevelTime::LevelTime(::LevelTime* level_time_)
  : level_time(level_time_)
{ }

LevelTime::~LevelTime()
{ }

void LevelTime::start()
{
  level_time->start();
}

void LevelTime::stop()
{
  level_time->stop();
}

float LevelTime::get_time() const
{
  return level_time->get_time();
}

void LevelTime::set_time(float time_left)
{
  level_time->set_time(time_left);
}

}

// EOF
