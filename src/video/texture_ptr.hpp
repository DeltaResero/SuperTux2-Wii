// src/video/texture_ptr.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2009 Ingo Ruhnke <grumbel@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_VIDEO_TEXTURE_PTR_HPP
#define HEADER_SUPERTUX_VIDEO_TEXTURE_PTR_HPP

#include <memory>

class Texture;
typedef std::shared_ptr<Texture> TexturePtr;

#endif // HEADER_SUPERTUX_VIDEO_TEXTURE_PTR_HPP

// EOF
