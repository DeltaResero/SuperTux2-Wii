// src/scripting/background.cpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2016 Hume2 <teratux.mail@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#include "scripting/background.hpp"

#include "util/file_system.hpp"

#include "object/background.hpp"

namespace scripting {

Background::Background(::Background* parent) :
  m_parent(parent)
{
}

Background::~Background()
{
}

void
Background::set_image(const std::string& image)
{
  if(image.empty())
  {
    log_info << "No filename / path for background image specified" << std::endl;
    return;
  }

const std::string& default_dir = "images/background";
  bool path_valid = true;

  if(!FileSystem::exists(image))
    path_valid = false;

  m_parent->set_image(path_valid ? image : FileSystem::join(default_dir, image));
}

void
Background::set_images(const std::string& top_image, const std::string& middle_image,
                              const std::string& bottom_image)
{
  if(top_image.empty() || middle_image.empty() || bottom_image.empty())
  {
    log_info << "No filename / path for background image specified" << std::endl;
    return;
  }

  const std::string& default_dir = "images/background";
  bool top_image_valid = true, middle_image_valid = true, bottom_image_valid = true;

  if(!FileSystem::exists(top_image))
    top_image_valid = false;

  if(!FileSystem::exists(middle_image))
    middle_image_valid = false;

  if(!FileSystem::exists(bottom_image))
    bottom_image_valid = false;

  m_parent->set_images(top_image_valid ? top_image : FileSystem::join(default_dir, top_image),
                       middle_image_valid ? middle_image : FileSystem::join(default_dir, middle_image),
                       bottom_image_valid ? bottom_image : FileSystem::join(default_dir, bottom_image));
}

void
Background::set_speed(float speed)
{
  m_parent->set_image(m_parent->get_image(), speed);
}

} // namespace scripting

// EOF
