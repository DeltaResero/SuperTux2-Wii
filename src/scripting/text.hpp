// src/scripting/text.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_SCRIPTING_TEXT_HPP
#define HEADER_SUPERTUX_SCRIPTING_TEXT_HPP

#ifndef SCRIPTING_API
#include <string>

class TextObject;
#endif

namespace scripting {

class Text
{
#ifndef SCRIPTING_API
private:
  ::TextObject* m_parent;

public:
  Text(::TextObject* parent);
  ~Text();

private:
  Text(const Text&) = delete;
  Text& operator=(const Text&) = delete;
#endif

public:
  void set_text(const std::string& text);
  void set_font(const std::string& fontname);
  void fade_in(float fadetime);
  void fade_out(float fadetime);
  void set_visible(bool visible);
  void set_centered(bool centered);
  void set_pos(float x, float y);
  float get_pos_x() const;
  float get_pos_y() const;
  void set_anchor_point(int anchor);
  int  get_anchor_point() const;
};

}

#endif // HEADER_SUPERTUX_SCRIPTING_TEXT_HPP

// EOF
