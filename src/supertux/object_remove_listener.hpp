// src/supertux/object_remove_listener.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_SUPERTUX_OBJECT_REMOVE_LISTENER_HPP
#define HEADER_SUPERTUX_SUPERTUX_OBJECT_REMOVE_LISTENER_HPP

class GameObject;

class ObjectRemoveListener
{
public:
  virtual ~ObjectRemoveListener()
  {}

  virtual void object_removed(GameObject* object) = 0;
};

#endif // HEADER_SUPERTUX_SUPERTUX_OBJECT_REMOVE_LISTENER_HPP

// EOF
