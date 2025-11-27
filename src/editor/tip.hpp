// src/editor/tip.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2015 Hume2 <teratux.mail@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_EDITOR_TIP_HPP
#define HEADER_SUPERTUX_EDITOR_TIP_HPP

#include "video/surface.hpp"
#include "video/drawing_context.hpp"

class GameObject;

class Tip
{
  public:
    Tip(GameObject* object);
    ~Tip();

    void draw(DrawingContext& context, Vector pos);
    void draw_up(DrawingContext& context, Vector pos);

  private:
    std::vector<std::string> strings;
    std::string header;
};

#endif // HEADER_SUPERTUX_EDITOR_TIP_HPP

// EOF
