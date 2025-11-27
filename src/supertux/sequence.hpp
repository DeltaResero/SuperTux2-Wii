// src/supertux/sequence.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2016 Hume2 <teratux.mail@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_SUPERTUX_SEQUENCE_HPP
#define HEADER_SUPERTUX_SUPERTUX_SEQUENCE_HPP

enum Sequence {
  SEQ_ENDSEQUENCE,
  SEQ_STOPTUX,
  SEQ_FIREWORKS
};

Sequence string_to_sequence(const std::string& sequencename);
std::string sequence_to_string(const Sequence& seq);

#endif // HEADER_SUPERTUX_SUPERTUX_SEQUENCE_HPP

// EOF
