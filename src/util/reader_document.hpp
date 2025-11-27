// src/util/reader_document.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2015 Ingo Ruhnke <grumbel@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_UTIL_READER_DOCUMENT_HPP
#define HEADER_SUPERTUX_UTIL_READER_DOCUMENT_HPP

#include <istream>
#include <sexp/value.hpp>

#include "util/reader_object.hpp"

/** The ReaderDocument holds the memory */
class ReaderDocument final
{
public:
  static ReaderDocument parse(std::istream& stream, const std::string& filename = "<stream>");
  static ReaderDocument parse(const std::string& filename);

public:
  ReaderDocument();
  ReaderDocument(const std::string& filename, sexp::Value sx);

  ReaderObject get_root() const;
  std::string get_filename() const;

private:
  std::string m_filename;
  sexp::Value m_sx;
};

#endif // HEADER_SUPERTUX_UTIL_READER_DOCUMENT_HPP

// EOF
