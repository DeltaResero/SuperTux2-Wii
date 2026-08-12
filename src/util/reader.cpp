// src/util/reader.cpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
//  Utility functions for config handling.
//  Copyright (C) 2010  Florian Forster
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//  Authors:
//    Florian "octo" Forster <supertux at octo.it>

#include "util/reader.hpp"

#include <fstream>
#include <sexp/io.hpp>
#include <sexp/parser.hpp>
#include <sexp/util.hpp>

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

/* EOF */
