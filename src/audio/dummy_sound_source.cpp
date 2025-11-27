// src/audio/dummy_sound_source.cpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
// SuperTux
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#include "audio/dummy_sound_source.hpp"

#include <memory>

#include "audio/sound_source.hpp"

class DummySoundSource : public SoundSource
{
public:
  DummySoundSource() :
    is_playing(false)
  {}
  virtual ~DummySoundSource()
  {}

  virtual void play()
  {
    is_playing = true;
  }

  virtual void stop()
  {
    is_playing = false;
  }

  virtual bool playing() const
  {
    return is_playing;
  }

  virtual void set_looping(bool )
  {
  }

  virtual void set_relative(bool )
  {
  }

  virtual void set_gain(float )
  {
  }

  virtual void set_pitch(float )
  {
  }

  virtual void set_position(const Vector& )
  {
  }

  virtual void set_velocity(const Vector& )
  {
  }

  virtual void set_reference_distance(float )
  {
  }

private:
  bool is_playing;

private:
  DummySoundSource(const DummySoundSource&) = delete;
  DummySoundSource& operator=(const DummySoundSource&) = delete;
};

std::unique_ptr<SoundSource> create_dummy_sound_source()
{
  return std::unique_ptr<SoundSource>(new DummySoundSource);
}

// EOF
