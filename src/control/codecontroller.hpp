// src/control/codecontroller.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_CONTROL_CODECONTROLLER_HPP
#define HEADER_SUPERTUX_CONTROL_CODECONTROLLER_HPP

#include "control/controller.hpp"

/**
 * This is a dummy controller that doesn't react to any user input but should
 * be controlled by code
 */
class CodeController : public Controller
{
public:
  CodeController();
  virtual ~CodeController();

  void press(Control c, bool pressed = true);
  void update();
};

#endif // HEADER_SUPERTUX_CONTROL_CODECONTROLLER_HPP

// EOF
