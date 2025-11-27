// src/util/utf8_iterator.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux - Ingo Ruhnke <grumbel@gmail.com>
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_UTIL_UTF8_ITERATOR_HPP
#define HEADER_SUPERTUX_UTIL_UTF8_ITERATOR_HPP

#include <string>
#include <stdint.h>

class UTF8Iterator
{
public:
  const std::string&     text;
  std::string::size_type pos;
  uint32_t chr;

  UTF8Iterator(const std::string& text_);

  bool done() const;
  UTF8Iterator& operator++();
  uint32_t operator*() const;
};

#endif // HEADER_SUPERTUX_UTIL_UTF8_ITERATOR_HPP

// EOF
