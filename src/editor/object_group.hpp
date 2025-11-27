// src/editor/object_group.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2015 Hume2 <teratux.mail@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_EDITOR_OBJECT_GROUP_HPP
#define HEADER_SUPERTUX_EDITOR_OBJECT_GROUP_HPP

#include <string>
#include <vector>

#include "editor/object_icon.hpp"

#include "util/reader_fwd.hpp"

class ObjectGroup
{
  public:
    ObjectGroup();
    ObjectGroup(const ReaderMapping& reader);
    ~ObjectGroup();

    std::string name;
    std::vector<ObjectIcon> icons;
    bool for_worldmap;

    void add_icon(const std::string& object, const std::string& icon_path);
};

#endif // HEADER_SUPERTUX_EDITOR_OBJECT_GROUP_HPP

// EOF
