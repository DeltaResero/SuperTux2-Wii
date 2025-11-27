// src/scripting/scripted_object.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_SCRIPTING_SCRIPTED_OBJECT_HPP
#define HEADER_SUPERTUX_SCRIPTING_SCRIPTED_OBJECT_HPP

#ifndef SCRIPTING_API
#include <string>

class ScriptedObject;
#endif

namespace scripting {

class ScriptedObject
{
#ifndef SCRIPTING_API
private:
  ::ScriptedObject* m_parent;

public:
  ScriptedObject(::ScriptedObject* parent);
  ~ScriptedObject();

private:
  ScriptedObject(const ScriptedObject&) = delete;
  ScriptedObject& operator=(const ScriptedObject&) = delete;
#endif

public:
  void set_action(const std::string& animation);
  std::string get_action() const;

  void move(float x, float y);
  void set_pos(float x, float y);
  float get_pos_x() const;
  float get_pos_y() const;

  void set_velocity(float x, float y);
  float get_velocity_x() const;
  float get_velocity_y() const;

  void enable_gravity(bool f);
  bool gravity_enabled() const;

  void set_visible(bool visible);
  bool is_visible() const;

  void set_solid(bool solid);
  bool is_solid() const;

  std::string get_name() const;
};

} // namespace scripting

#endif // HEADER_SUPERTUX_SCRIPTING_SCRIPTED_OBJECT_HPP

// EOF
