// src/util/reader_object.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2015 Ingo Ruhnke <grumbel@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_UTIL_READER_OBJECT_HPP
#define HEADER_SUPERTUX_UTIL_READER_OBJECT_HPP

#include <string>

namespace sexp {
class Value;
} // namespace sexp

class ReaderDocument;
class ReaderMapping;
class ReaderCollection;

class ReaderObject final
{
public:
  ReaderObject();
  ReaderObject(const ReaderDocument* doc, const sexp::Value* sx);

  std::string get_name() const;
  ReaderMapping get_mapping() const;
  ReaderCollection get_collection() const;

private:
  const ReaderDocument* m_doc;
  const sexp::Value* m_sx;
};

#endif // HEADER_SUPERTUX_UTIL_READER_OBJECT_HPP

// EOF
