// src/scripting/candle.cpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#include "object/candle.hpp"
#include "scripting/candle.hpp"

namespace scripting {

Candle::Candle(::Candle* candle_)
  : candle(candle_)
{ }

Candle::~Candle()
{ }

bool Candle::get_burning() const
{
  return candle->get_burning();
}

void Candle::set_burning(bool burning)
{
  candle->set_burning(burning);
}

}

// EOF
