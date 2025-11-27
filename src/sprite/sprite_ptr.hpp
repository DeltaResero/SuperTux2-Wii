// src/sprite/sprite_ptr.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2009 Ingo Ruhnke <grumbel@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_SPRITE_SPRITE_PTR_HPP
#define HEADER_SUPERTUX_SPRITE_SPRITE_PTR_HPP

#include <memory>

class Sprite;

typedef std::shared_ptr<Sprite> SpritePtr;

#endif // HEADER_SUPERTUX_SPRITE_SPRITE_PTR_HPP

// EOF
