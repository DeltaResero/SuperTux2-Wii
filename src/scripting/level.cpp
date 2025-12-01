// src/scripting/level.cpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#include "supertux/flip_level_transformer.hpp"
#include "supertux/game_session.hpp"

namespace scripting {
void
Level_finish(bool win)
{
  if(GameSession::current() == NULL)
    return;

  GameSession::current()->finish(win);
}

void
Level_spawn(const std::string& sector, const std::string& spawnpoint)
{
  if(GameSession::current() == NULL)
    return;

  GameSession::current()->respawn(sector, spawnpoint);
}

void
Level_flip_vertically()
{
  FlipLevelTransformer flip_transformer;
  flip_transformer.transform(GameSession::current()->get_current_level());
}

void
Level_toggle_pause()
{
  if(GameSession::current() == NULL)
    return;
  GameSession::current()->toggle_pause();
}

}

// EOF
