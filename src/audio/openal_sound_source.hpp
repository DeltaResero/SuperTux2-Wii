// src/audio/openal_sound_source.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_SUPERTUX_AUDIO_OPENAL_SOUND_SOURCE_HPP
#define HEADER_SUPERTUX_AUDIO_OPENAL_SOUND_SOURCE_HPP

#include <config.h>

#ifdef HAVE_OPENAL
#include <al.h>

#include "audio/sound_source.hpp"

class OpenALSoundSource : public SoundSource
{
public:
  OpenALSoundSource();
  virtual ~OpenALSoundSource();

  virtual void play();
  virtual void stop();
  virtual void pause();
  virtual void resume();
  virtual bool playing() const;
  virtual bool paused() const;

  virtual void update();

  virtual void set_looping(bool looping);
  virtual void set_relative(bool relative);
  virtual void set_gain(float gain);
  virtual void set_pitch(float pitch);
  virtual void set_position(const Vector& position);
  virtual void set_velocity(const Vector& position);
  virtual void set_reference_distance(float distance);

protected:
  friend class SoundManager;

  ALuint source;

private:
  OpenALSoundSource(const OpenALSoundSource&) = delete;
  OpenALSoundSource& operator=(const OpenALSoundSource&) = delete;
};

#endif // HAVE_OPENAL

#endif // HEADER_SUPERTUX_AUDIO_OPENAL_SOUND_SOURCE_HPP

// EOF
