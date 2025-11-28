// src/util/gettext.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Ingo Ruhnke <grumbel@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_UTIL_GETTEXT_HPP
#define HEADER_SUPERTUX_UTIL_GETTEXT_HPP

#include <string>

// Fallback for files that haven't been refactored yet.
// For the Wii port, we want to eventually remove this and use raw strings everywhere.
static inline std::string _(const std::string& message)
{
  return message;
}

#endif // HEADER_SUPERTUX_UTIL_GETTEXT_HPP

// EOF
