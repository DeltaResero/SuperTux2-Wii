// src/object/endsequence.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux - End Sequence
// Copyright (C) 2007 Christoph Sommer <christoph.sommer@2006.expires.deltadevelopment.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_OBJECT_ENDSEQUENCE_HPP
#define HEADER_SUPERTUX_OBJECT_ENDSEQUENCE_HPP

#include "control/codecontroller.hpp"
#include "supertux/game_object.hpp"

class EndSequence : public GameObject
{
public:
  EndSequence();
  virtual ~EndSequence();

  virtual void update(float elapsed_time);
  virtual void draw(DrawingContext& context);

  void start(); /**< play EndSequence */
  void stop_tux(); /**< called when Tux has reached his final position */
  void stop(); /**< stop playing EndSequence, mark it as done playing */
  bool is_tux_stopped() const; /**< returns true if Tux has reached his final position */
  bool is_done() const; /**< returns true if EndSequence has finished playing */
  virtual bool do_save() const {
    return false;
  }

protected:
  virtual void starting(); /**< called when EndSequence starts */
  virtual void running(float elapsed_time); /**< called while the EndSequence is running */
  virtual void stopping(); /**< called when EndSequence stops */

protected:
  bool isrunning; /**< true while EndSequence plays */
  bool isdone; /**< true if EndSequence has finished playing */
  bool tux_may_walk; /**< true while tux is allowed to walk */
  std::unique_ptr<CodeController> end_sequence_controller;

private:
  EndSequence(const EndSequence&);
  EndSequence& operator=(const EndSequence&);
};

#endif // HEADER_SUPERTUX_OBJECT_ENDSEQUENCE_HPP

// EOF
