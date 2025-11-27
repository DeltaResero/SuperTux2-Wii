// src/control/codecontroller.cpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#include "control/codecontroller.hpp"

CodeController::CodeController()
{}

CodeController::~CodeController()
{}

void
CodeController::press(Control c, bool pressed_)
{
  controls[c] = pressed_;
}

void
CodeController::update()
{
  Controller::update();

  for(int i = 0; i < CONTROLCOUNT; ++i)
    controls[i] = false;
}

// EOF
