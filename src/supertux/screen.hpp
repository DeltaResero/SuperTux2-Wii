// src/supertux/screen.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_SUPERTUX_SCREEN_HPP
#define HEADER_SUPERTUX_SUPERTUX_SCREEN_HPP

class DrawingContext;

/**
 * Abstract base class for code the MainLoop runs exclusively and full-screen.
 *
 * Examples of Screens are: The TitleScreen, a WorldMap, a level's
 * GameSession, a TextScroller, ...
 */
class Screen
{
public:
  virtual ~Screen()
  {}

  /**
   * gets called before this screen gets activated (which is at least once
   * before the first draw or update call
   */
  virtual void setup()
  {}
  /** gets called when the current screen is temporarily suspended */
  virtual void leave()
  {}

  /**
   * gets called once per frame. The screen should draw itself in this function.
   * State changes should not be done in this function, but rather in update
   */
  virtual void draw(DrawingContext& context) = 0;

  /**
   * gets called for once (per logical) frame. Screens should do their state
   * updates and logic here
   */
  virtual void update(float elapsed_time) = 0;
};

#endif // HEADER_SUPERTUX_SUPERTUX_SCREEN_HPP

// EOF
