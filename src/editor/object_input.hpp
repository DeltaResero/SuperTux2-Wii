// src/editor/object_input.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2015 Hume2 <teratux.mail@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_EDITOR_OBJECT_INPUT_HPP
#define HEADER_SUPERTUX_EDITOR_OBJECT_INPUT_HPP

#include "editor/object_group.hpp"

class ObjectInput
{
  public:
    ObjectInput();
    ~ObjectInput();

    std::vector<ObjectGroup> groups;
};

#endif // HEADER_SUPERTUX_EDITOR_OBJECT_INPUT_HPP

// EOF
