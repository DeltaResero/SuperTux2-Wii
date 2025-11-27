// src/video/sdl/sdl_surface_data.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_VIDEO_SDL_SURFACE_DATA_HPP
#define HEADER_SUPERTUX_VIDEO_SDL_SURFACE_DATA_HPP

#include <config.h>

#include "supertux/gameconfig.hpp"
#include "supertux/globals.hpp"
#include "video/surface.hpp"
#include "video/surface_data.hpp"
#include "video/texture.hpp"

class SDLSurfaceData : public SurfaceData
{
private:

public:
  SDLSurfaceData(const Surface &surface)
  {
  }
};

#endif // HEADER_SUPERTUX_VIDEO_SDL_SURFACE_DATA_HPP

// EOF
