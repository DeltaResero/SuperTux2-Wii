// src/object/endsequence_fireworks.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux - End Sequence: Tux walks right
// Copyright (C) 2007 Christoph Sommer <christoph.sommer@2006.expires.deltadevelopment.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_OBJECT_ENDSEQUENCE_FIREWORKS_HPP
#define HEADER_SUPERTUX_OBJECT_ENDSEQUENCE_FIREWORKS_HPP

#include "object/endsequence.hpp"
#include "supertux/timer.hpp"

class EndSequenceFireworks : public EndSequence
{
public:
  EndSequenceFireworks();
  virtual ~EndSequenceFireworks();
  virtual void draw(DrawingContext& context);

protected:
  virtual void starting(); /**< called when EndSequence starts */
  virtual void running(float elapsed_time); /**< called while the EndSequence is running */
  virtual void stopping(); /**< called when EndSequence stops */

  Timer endsequence_timer;
};

#endif // HEADER_SUPERTUX_OBJECT_ENDSEQUENCE_FIREWORKS_HPP

// EOF
