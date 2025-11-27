// src/scripting/gradient.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux - Sector scripting
// Copyright (C) 2016 Hume2 <teratux.mail@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_SCRIPTING_GRADIENT_HPP
#define HEADER_SUPERTUX_SCRIPTING_GRADIENT_HPP

#ifndef SCRIPTING_API
#include <string>
class Gradient;
#endif

namespace scripting {

class Gradient
{
#ifndef SCRIPTING_API
private:
  ::Gradient* gradient;

public:
  Gradient(::Gradient* parent);
  ~Gradient();

private:
  Gradient(const Gradient&) = delete;
  Gradient& operator=(const Gradient&) = delete;
#endif

public:
  void set_direction(const std::string& direction);
  std::string get_direction() const;

  void set_color1(float red, float green, float blue);
  void set_color2(float red, float green, float blue);

  void swap_colors();
};

}

#endif // HEADER_SUPERTUX_SCRIPTING_GRADIENT_HPP

// EOF
