// src/util/reader.cpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux - Utility functions for config handling.
// Copyright (C) 2010  Florian Forster
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#include "util/reader.hpp"

#include <fstream>
#include <physfs.h>
#include <sexp/io.hpp>
#include <sexp/parser.hpp>
#include <sexp/util.hpp>

#include "util/gettext.hpp"
#include "util/reader_mapping.hpp"
#include "video/drawing_request.hpp"

int reader_get_layer(const ReaderMapping& reader, int def)
{
  int tmp = 0;
  bool status;

  status = reader.get("z-pos", tmp);

  if (!status)
    status = reader.get("layer", tmp);

  if (!status)
    tmp = def;

  if (tmp > (LAYER_GUI - 100))
    tmp = LAYER_GUI - 100;

  return (tmp);
}

namespace {

std::string dirname(const std::string& filename)
{
  std::string::size_type p = filename.find_last_of('/');
  if(p == std::string::npos) {
    return {};
  } else {
    return filename.substr(0, p);
  }
}

} // namespace

void register_translation_directory(const std::string& filename)
{
  // Translation support removed
}

// EOF
