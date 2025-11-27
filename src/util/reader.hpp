// src/util/reader.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2015 Ingo Ruhnke <grumbel@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_UTIL_READER_HPP
#define HEADER_SUPERTUX_UTIL_READER_HPP

#include <memory>
#include <vector>
#include <string>
#include <sexp/value.hpp>

#include "util/reader_fwd.hpp"

int reader_get_layer(const ReaderMapping& reader, int def);
void register_translation_directory(const std::string& filename);

#endif // HEADER_SUPERTUX_UTIL_READER_HPP

// EOF
