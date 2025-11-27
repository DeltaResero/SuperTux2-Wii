// src/scripting/display_effect.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_SCRIPTING_DISPLAY_EFFECT_HPP
#define HEADER_SUPERTUX_SCRIPTING_DISPLAY_EFFECT_HPP

#ifndef SCRIPTING_API
class DisplayEffect;
#endif

namespace scripting {

class DisplayEffect
{
#ifndef SCRIPTING_API
private:
  ::DisplayEffect* m_parent;

public:
  DisplayEffect(::DisplayEffect* parent);
  ~DisplayEffect();

private:
private:
  DisplayEffect(const DisplayEffect&) = delete;
  DisplayEffect& operator=(const DisplayEffect&) = delete;
#endif

public:
  /// fade display to black
  void fade_out(float fadetime);
  /// fade display from black to normal
  void fade_in(float fadetime);
  /// set display black (or back to normal)
  void set_black(bool enabled);
  /// check if display is set to black
  bool is_black() const;
  /// set black borders for cutscenes
  void sixteen_to_nine(float fadetime);
  /// deactivate borders
  void four_to_three(float fadetime);

  // fade display until just a small visible circle is left
  // (like what happens in some cartoons at the end)
  // void shrink_fade(Vector goal, float radius, float fadetime);
};

}

#endif // HEADER_SUPERTUX_SCRIPTING_DISPLAY_EFFECT_HPP

// EOF
