// src/util/reader_iterator.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2015 Ingo Ruhnke <grumbel@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_UTIL_READER_ITERATOR_HPP
#define HEADER_SUPERTUX_UTIL_READER_ITERATOR_HPP

#include <string>
#include <vector>

namespace sexp {
class Value;
} // namespace sexp

class ReaderMapping;
class ReaderDocument;

/** The ReaderIterator class is for backward compatibilty with old
    fileformats only, do not use it in new code, use ReaderCollection
    and ReaderMapping instead */
class ReaderIterator final
{
public:
  // sx should point to (section (name value)...)
  ReaderIterator(const ReaderDocument* doc, const sexp::Value* sx);

  /** must be called once before any of the other function become
      valid, i.e. ReaderIterator it; while(it.next()) { ... } */
  bool next();

  bool is_string();
  bool is_pair();
  std::string as_string_item();

  std::string get_key() const;

  void get(bool& value) const;
  void get(int& value) const;
  void get(float& value) const;
  void get(std::string& value) const;

  ReaderMapping as_mapping() const;

private:
  const ReaderDocument* m_doc;
  const std::vector<sexp::Value>& m_arr;
  size_t m_idx;
};

#endif // HEADER_SUPERTUX_UTIL_READER_ITERATOR_HPP

// EOF
