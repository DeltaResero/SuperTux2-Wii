// src/video/sdl_surface_ptr.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2009 Ingo Ruhnke <grumbel@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_VIDEO_SDL_SURFACE_PTR_HPP
#define HEADER_SUPERTUX_VIDEO_SDL_SURFACE_PTR_HPP

#include <SDL.h>

/** Simple Wrapper class around SDL_Surface that provides execption
    safety */
class SDLSurfacePtr
{
private:
  SDL_Surface* m_surface;

public:
  SDLSurfacePtr() :
    m_surface(0)
  {}

  SDLSurfacePtr(SDL_Surface* surface) :
    m_surface(surface)
  {}

  ~SDLSurfacePtr()
  {
    SDL_FreeSurface(m_surface);
  }

  SDL_Surface* operator->()
  {
    return m_surface;
  }

  void reset(SDL_Surface* surface)
  {
    SDL_FreeSurface(m_surface);
    m_surface = surface;
  }

  SDL_Surface* get()
  {
    return m_surface;
  }

  operator void*() {
    return m_surface;
  }

private:
  SDLSurfacePtr(const SDLSurfacePtr&);
  SDLSurfacePtr& operator=(const SDLSurfacePtr&);
};

#endif // HEADER_SUPERTUX_VIDEO_SDL_SURFACE_PTR_HPP

// EOF
