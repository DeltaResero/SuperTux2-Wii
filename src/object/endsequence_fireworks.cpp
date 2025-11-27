// src/object/endsequence_fireworks.cpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux - End Sequence: Tux walks right
// Copyright (C) 2007 Christoph Sommer <christoph.sommer@2006.expires.deltadevelopment.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#include "object/endsequence_fireworks.hpp"

#include "object/fireworks.hpp"
#include "supertux/globals.hpp"
#include "supertux/screen_manager.hpp"
#include "supertux/sector.hpp"

EndSequenceFireworks::EndSequenceFireworks() :
  EndSequence(),
  endsequence_timer()
{
}

EndSequenceFireworks::~EndSequenceFireworks()
{
}

void
EndSequenceFireworks::draw(DrawingContext& /*context*/)
{
}

void
EndSequenceFireworks::starting()
{
  EndSequence::starting();
  endsequence_timer.start(7.3f * ScreenManager::current()->get_speed());
  Sector::current()->add_object(std::make_shared<Fireworks>());
}

void
EndSequenceFireworks::running(float elapsed_time)
{
  EndSequence::running(elapsed_time);
  //Player& tux = *Sector::current()->player;

  if (tux_may_walk) {
    end_sequence_controller->press(Controller::JUMP);
  }

  if (endsequence_timer.check()) isdone = true;
}

void
EndSequenceFireworks::stopping()
{
  EndSequence::stopping();
}

// EOF
