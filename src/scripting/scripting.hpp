// src/scripting/scripting.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2014 Ingo Ruhnke <grumbel@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_SCRIPTING_SCRIPTING_HPP
#define HEADER_SUPERTUX_SCRIPTING_SCRIPTING_HPP

#include <squirrel.h>

#include "util/currenton.hpp"

namespace scripting {

extern HSQUIRRELVM global_vm;

class Scripting : public Currenton<Scripting>
{
private:
public:
  Scripting(bool enable_debugger);
  ~Scripting();

  void update_debugger();

private:
  Scripting(const Scripting&) = delete;
  Scripting& operator=(const Scripting&) = delete;
};

} // namespace scripting

#endif // HEADER_SUPERTUX_SCRIPTING_SCRIPTING_HPP

// EOF
