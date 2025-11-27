// src/scripting/background.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux - Sector scripting
// Copyright (C) 2016 Hume2 <teratux.mail@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_SCRIPTING_BACKGROUND_HPP
#define HEADER_SUPERTUX_SCRIPTING_BACKGROUND_HPP

#ifndef SCRIPTING_API
#include <string>
class Background;
#endif

namespace scripting {

class Background
{
#ifndef SCRIPTING_API
private:
  ::Background* m_parent;

public:
  Background(::Background* parent);
  ~Background();

private:
  Background(const Background&) = delete;
  Background& operator=(const Background&) = delete;
#endif

public:
  void set_image(const std::string& image);
  void set_images(const std::string& top_image, const std::string& middle_image,
                             const std::string& bottom_image);
  void set_speed(float speed);
};

}

#endif // HEADER_SUPERTUX_SCRIPTING_BACKGROUND_HPP

// EOF
