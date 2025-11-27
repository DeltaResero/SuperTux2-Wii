// src/scripting/platform.cpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#include "object/platform.hpp"
#include "scripting/platform.hpp"

#define NOIMPL      log_fatal << __PRETTY_FUNCTION__ << " not implemented."

namespace scripting {

Platform::Platform(::Platform* platform_)
  : platform(platform_)
{ }

Platform::~Platform()
{ }

void Platform::goto_node(int node_no)
{
  platform->goto_node(node_no);
}

void Platform::start_moving()
{
  platform->start_moving();
}

void Platform::stop_moving()
{
  platform->stop_moving();
}

}

// EOF
