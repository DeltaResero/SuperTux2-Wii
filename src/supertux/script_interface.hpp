// src/supertux/script_interface.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_SUPERTUX_SCRIPT_INTERFACE_HPP
#define HEADER_SUPERTUX_SUPERTUX_SCRIPT_INTERFACE_HPP

#include <squirrel.h>

/**
 * Objects that want to expose themself to the scripting environment
 * should implement this interface
 */
class ScriptInterface
{
public:
  virtual ~ScriptInterface()
  {}

  virtual void expose(HSQUIRRELVM vm, SQInteger table_idx) = 0;
  virtual void unexpose(HSQUIRRELVM vm, SQInteger table_idx) = 0;
};

#endif // HEADER_SUPERTUX_SUPERTUX_SCRIPT_INTERFACE_HPP

// EOF
