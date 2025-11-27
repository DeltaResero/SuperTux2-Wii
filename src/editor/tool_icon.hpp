// src/editor/tool_icon.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2015 Hume2 <teratux.mail@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_EDITOR_TOOL_ICON_HPP
#define HEADER_SUPERTUX_EDITOR_TOOL_ICON_HPP

#include "util/reader_fwd.hpp"
#include "video/surface.hpp"
#include "video/drawing_context.hpp"

class ToolIcon
{
  public:
    ToolIcon(const std::string& icon);
    ~ToolIcon();

    Vector pos;

    void draw(DrawingContext& context);

    int get_mode() const {
      return mode;
    }

    void next_mode();

    void push_mode(const std::string& icon);
    SurfacePtr get_current_surface() const;

  private:
    std::vector<SurfacePtr> surfaces;
    int mode;
    int surf_count;

    ToolIcon(const ToolIcon&);
    ToolIcon& operator=(const ToolIcon&);
};

#endif // HEADER_SUPERTUX_EDITOR_TOOL_ICON_HPP

// EOF
