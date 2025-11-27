// src/util/writer.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_UTIL_WRITER_HPP
#define HEADER_SUPERTUX_UTIL_WRITER_HPP

#include <string>
#include <vector>

class Writer
{
public:
  Writer(const std::string& filename);
  Writer(std::ostream* out);
  ~Writer();

  void write_comment(const std::string& comment);

  void start_list(const std::string& listname, bool string = false);

  void write(const std::string& name, bool value);
  void write(const std::string& name, int value);
  void write(const std::string& name, float value);
  void write(const std::string& name, const char* value);
  void write(const std::string& name, const std::string& value, bool translatable = false);
  void write(const std::string& name, const std::vector<int>& value);
  void write(const std::string& name, const std::vector<unsigned int>& value);
  void write(const std::string& name, const std::vector<float>& value);
  void write(const std::string& name, const std::vector<std::string>& value);
  // add more write-functions when needed...

  void end_list(const std::string& listname);

private:
  void write_escaped_string(const std::string& str);
  void indent();

private:
  std::ostream* out;
  bool out_owned;
  int indent_depth;
  std::vector<std::string> lists;

private:
  Writer(const Writer&);
  Writer & operator=(const Writer&);
};

#endif // HEADER_SUPERTUX_UTIL_WRITER_HPP

// EOF
