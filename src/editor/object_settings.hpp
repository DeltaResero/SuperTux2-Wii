// src/editor/object_settings.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2015 Hume2 <teratux.mail@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_EDITOR_OBJECT_SETTINGS_HPP
#define HEADER_SUPERTUX_EDITOR_OBJECT_SETTINGS_HPP

#include <vector>

#include "editor/object_option.hpp"

class ObjectSettings
{
  public:
    ObjectSettings(const std::string& name_);
    ~ObjectSettings();

    std::string name;
    std::vector<ObjectOption> options;

    void copy_from(ObjectSettings* other);
};

#endif // HEADER_SUPERTUX_EDITOR_OBJECT_SETTINGS_HPP

// EOF
