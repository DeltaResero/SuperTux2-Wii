// src/supertux/resources.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2003 Tobias Glaesser <tobi.web@gmx.de>
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_SUPERTUX_RESOURCES_HPP
#define HEADER_SUPERTUX_SUPERTUX_RESOURCES_HPP

#include <memory>

#include "video/font_ptr.hpp"
#include "video/surface_ptr.hpp"

class MouseCursor;

class Resources
{
public:
  static std::unique_ptr<MouseCursor> mouse_cursor;

  static FontPtr fixed_font;
  static FontPtr normal_font;
  static FontPtr small_font;
  static FontPtr big_font;

  static SurfacePtr checkbox;
  static SurfacePtr checkbox_checked;
  static SurfacePtr back;
  static SurfacePtr arrow_left;
  static SurfacePtr arrow_right;

public:
  Resources();
  ~Resources();
};

#endif // HEADER_SUPERTUX_SUPERTUX_RESOURCES_HPP

// EOF
