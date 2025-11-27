// src/editor/node_marker.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2016 Hume2 <teratux.mail@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_NODE_MARKER_HPP
#define HEADER_SUPERTUX_NODE_MARKER_HPP

#include "editor/point_marker.hpp"
#include "object/path.hpp"

//class Path;
//class Path::Node;

class NodeMarker : public PointMarker
{
  public:
    NodeMarker(Path* path_, std::vector<Path::Node>::iterator node_iterator, size_t id_);
    ~NodeMarker();

    Path* path;
    std::vector<Path::Node>::iterator node;
    size_t id;

    virtual void update(float elapsed_time);
    virtual void move_to(const Vector& pos);
    virtual void editor_delete();
    virtual Vector get_point_vector() const;
    virtual Vector get_offset() const;

    virtual ObjectSettings get_settings();

    void update_iterator();

  private:
    NodeMarker(const NodeMarker&);
    NodeMarker& operator=(const NodeMarker&);
};

#endif // HEADER_SUPERTUX_NODE_MARKER_HPP

// EOF
