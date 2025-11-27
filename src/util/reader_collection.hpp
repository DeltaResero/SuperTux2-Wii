// src/util/reader_collection.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2015 Ingo Ruhnke <grumbel@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_UTIL_READER_COLLECTION_HPP
#define HEADER_SUPERTUX_UTIL_READER_COLLECTION_HPP

#include <vector>

#include "util/reader_object.hpp"

namespace sexp {
class Value;
} // namespace sexp

class ReaderDocument;

class ReaderCollection final
{
public:
  ReaderCollection();

  // sx should point to (section (objname (name value)...)...)
  ReaderCollection(const ReaderDocument* doc, const sexp::Value* sx);

  std::vector<ReaderObject> get_objects() const;

private:
  const ReaderDocument* m_doc;
  const sexp::Value* m_sx;
};

#endif // HEADER_SUPERTUX_UTIL_READER_COLLECTION_HPP

// EOF
