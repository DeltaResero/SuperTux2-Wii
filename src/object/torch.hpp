// src/object/torch.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2014 Ingo Ruhnke <grumbel@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_OBJECT_TORCH_HPP
#define HEADER_SUPERTUX_OBJECT_TORCH_HPP

#include <memory>

#include "sprite/sprite_ptr.hpp"
#include "supertux/moving_object.hpp"

class ReaderMapping;

class Torch : public MovingObject
{
public:
  Torch(const ReaderMapping& reader);

  void draw(DrawingContext& context) override;
  void update(float) override;

  HitResponse collision(GameObject& other, const CollisionHit& ) override;
  std::string get_class() const override {
    return "torch";
  }
  std::string get_display_name() const override {
    return _("Torch");
  }

private:
  SpritePtr m_torch;
  SpritePtr m_flame;
  SpritePtr m_flame_glow;
  SpritePtr m_flame_light;
  bool m_burning;
  std::string sprite_name;

private:
  Torch(const Torch&) = delete;
  Torch& operator=(const Torch&) = delete;
};

#endif // HEADER_SUPERTUX_OBJECT_TORCH_HPP

// EOF
