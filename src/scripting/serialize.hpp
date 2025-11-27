// src/scripting/serialize.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_SCRIPTING_SERIALIZE_HPP
#define HEADER_SUPERTUX_SCRIPTING_SERIALIZE_HPP

#include <squirrel.h>

#include "util/reader_fwd.hpp"

class Writer;

namespace scripting {

void save_squirrel_table(HSQUIRRELVM vm, SQInteger table_idx, Writer& writer);
void load_squirrel_table(HSQUIRRELVM vm, SQInteger table_idx, const ReaderMapping& lisp);

} // namespace scripting

#endif // HEADER_SUPERTUX_SCRIPTING_SERIALIZE_HPP

// EOF
