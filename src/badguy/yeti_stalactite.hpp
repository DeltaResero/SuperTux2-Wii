// src/badguy/yeti_stalactite.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_BADGUY_YETI_STALACTITE_HPP
#define HEADER_SUPERTUX_BADGUY_YETI_STALACTITE_HPP

#include "badguy/stalactite.hpp"

class YetiStalactite : public Stalactite
{
public:
  YetiStalactite(const ReaderMapping& lisp);
  virtual ~YetiStalactite();

  void active_update(float elapsed_time);
  void start_shaking();
  bool is_hanging() const;
  void update(float elapsed_time);

  bool is_flammable() const;
  std::string get_class() const {
    return "yeti_stalactite";
  }
  std::string get_display_name() const {
    return _("Yeti's stalactite");
  }

};

#endif // HEADER_SUPERTUX_BADGUY_YETI_STALACTITE_HPP

// EOF
