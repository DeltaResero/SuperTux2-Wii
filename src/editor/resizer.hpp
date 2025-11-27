// src/editor/resizer.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2016 Hume2 <teratux.mail@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_EDITOR_RESIZER_HPP
#define HEADER_SUPERTUX_EDITOR_RESIZER_HPP

#include "editor/point_marker.hpp"

class Resizer : public PointMarker
{
  public:
    enum Side{
      NONE,
      LEFT_UP,
      RIGHT_DOWN
    };

    Resizer(Rectf* rect_, Side vert_, Side horz_);
    ~Resizer();

    void update(float elapsed_time);
    virtual void move_to(const Vector& pos);
    virtual Vector get_point_vector() const;
    virtual Vector get_offset() const;

    virtual bool do_save() const {
      return false;
    }

    void refresh_pos();

  private:
    Rectf* rect;
    Side vert;
    Side horz;

    Resizer(const Resizer&);
    Resizer& operator=(const Resizer&);
};

#endif // HEADER_SUPERTUX_EDITOR_RESIZER_HPP

// EOF
