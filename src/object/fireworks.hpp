// src/object/fireworks.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_OBJECT_FIREWORKS_HPP
#define HEADER_SUPERTUX_OBJECT_FIREWORKS_HPP

class Fireworks : public GameObject
{
public:
  Fireworks();
  ~Fireworks();

  virtual void update(float elapsed_time);
  virtual void draw(DrawingContext& context);

private:
  Timer timer;
};

#endif // HEADER_SUPERTUX_OBJECT_FIREWORKS_HPP

// EOF
