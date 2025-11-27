// src/scripting/wind.cpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#include "object/wind.hpp"
#include "scripting/wind.hpp"

#define NOIMPL      log_fatal << __PRETTY_FUNCTION__ << " not implemented."

namespace scripting {

Wind::Wind(::Wind* wind_)
  : wind(wind_)
{ }

Wind::~Wind()
{ }

void Wind::start()
{
  wind->start();
}

void Wind::stop()
{
  wind->stop();
}

}

// EOF
