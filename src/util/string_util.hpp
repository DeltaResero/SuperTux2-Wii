// src/util/string_util.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2009 Ingo Ruhnke <grumbel@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_UTIL_STRING_UTIL_HPP
#define HEADER_SUPERTUX_UTIL_STRING_UTIL_HPP

#include <string>

class StringUtil
{
public:
  static bool has_suffix(const std::string& data, const std::string& suffix);

  /** Compare two strings according to their numeric value, similar to
      what 'sort -n' does. */
  static bool numeric_less(const std::string& lhs, const std::string& rhs);
};

#endif // HEADER_SUPERTUX_UTIL_STRING_UTIL_HPP

// EOF
