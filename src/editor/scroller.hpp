// src/editor/scroller.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2016 Hume2 <teratux.mail@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_EDITOR_SCROLLER_HPP
#define HEADER_SUPERTUX_EDITOR_SCROLLER_HPP

#include <stdexcept>

#include "control/input_manager.hpp"
#include "math/vector.hpp"
#include "supertux/screen.hpp"

class LayerIcon;
class GameObject;
class Vector;
class Tip;

class EditorScroller
{
  public:
    EditorScroller();
    ~EditorScroller();

    void draw(DrawingContext&);
    void update(float elapsed_time);
    bool event(SDL_Event& ev);

    static bool rendered;

  private:
    bool scrolling;
    Vector scrolling_vec;
    Vector mouse_pos;

    void draw_arrow(DrawingContext&, Vector pos);
    bool can_scroll();

    EditorScroller(const EditorScroller&);
    EditorScroller& operator=(const EditorScroller&);
};

#endif // HEADER_SUPERTUX_EDITOR_SCROLLER_HPP

// EOF
