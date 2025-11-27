// src/object/bonus_block.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2009 Ingo Ruhnke <grumbel@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_OBJECT_BONUS_BLOCK_HPP
#define HEADER_SUPERTUX_OBJECT_BONUS_BLOCK_HPP

#include "object/block.hpp"
#include "object/player.hpp"

class BonusBlock : public Block
{
public:
  BonusBlock(const Vector& pos, int data);
  BonusBlock(const ReaderMapping& lisp);
  virtual ~BonusBlock();
  HitResponse collision(GameObject& other, const CollisionHit& hit);
  virtual void save(Writer& writer);

  void try_open(Player *player);
  void try_drop(Player *player);
  std::string get_class() const {
    return "bonusblock";
  }
  std::string get_display_name() const {
    return _("Bonus block");
  }
  enum Contents {
    CONTENT_COIN,
    CONTENT_FIREGROW,
    CONTENT_ICEGROW,
    CONTENT_AIRGROW,
    CONTENT_EARTHGROW,
    CONTENT_STAR,
    CONTENT_1UP,
    CONTENT_CUSTOM,
    CONTENT_SCRIPT,
    CONTENT_LIGHT,
    CONTENT_TRAMPOLINE,
    CONTENT_RAIN,
    CONTENT_EXPLODE
  };

  virtual ObjectSettings get_settings();

protected:
  virtual void hit(Player& player);

public:
  Contents contents;
  std::shared_ptr<MovingObject> object;
  int hit_counter;
  void draw(DrawingContext& context);

private:
  BonusBlock(const BonusBlock&);
  BonusBlock& operator=(const BonusBlock&);
  std::string script;
  SurfacePtr lightsprite;
  void get_content_by_data(int d);
  void raise_growup_bonus(Player* player, const BonusType& bonus, const Direction& dir);
  void drop_growup_bonus(const std::string& bonus_sprite_name, bool& countdown);
  BonusBlock::Contents get_content_from_string(const std::string& contentstring) const;
};

#endif // HEADER_SUPERTUX_OBJECT_BONUS_BLOCK_HPP

// EOF
