// src/supertux/timer.cpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#include <math.h>

#include "supertux/timer.hpp"

Timer::Timer() :
  period(0),
cycle_start(0),
  cyclic(false)
{
}

Timer::~Timer()
{
}

void
Timer::start(float period_, bool cyclic_)
{
  this->period = period_;
  this->cyclic = cyclic_;
  cycle_start = game_time;
}

bool
Timer::check()
{
  if(period == 0)
    return false;

  if(game_time - cycle_start >= period) {
    if(cyclic) {
      cycle_start = game_time - fmodf(game_time - cycle_start, period);
    } else {
      period = 0;
    }
    return true;
  }

  return false;
}

// EOF
