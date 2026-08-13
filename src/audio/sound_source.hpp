// src/audio/sound_source.hpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
//  SuperTux
//  Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef HEADER_SUPERTUX_AUDIO_SOUND_SOURCE_HPP
#define HEADER_SUPERTUX_AUDIO_SOUND_SOURCE_HPP

class Vector;

/**
 * A sound source represents the source of audio output. You can place
 * sources at certain points in your world or set their velocity to produce
 * doppler effects
 */
class SoundSource
{
public:
  SoundSource() {}
  virtual ~SoundSource() {}

  virtual void play() = 0;
  virtual void stop() = 0;
  virtual bool playing() const = 0;
  virtual void pause() = 0;
  virtual void resume() = 0;
  virtual bool paused() const = 0;

  /** Housekeeping between frames. Streaming sources refill here; the rest
      have nothing to do. */
  virtual void update() = 0;

  virtual void set_looping(bool looping) = 0;
  virtual void set_relative(bool relative) = 0;
  /// Set volume (0.0 is silent, 1.0 is normal)
  virtual void set_gain(float gain) = 0;
  virtual void set_pitch(float pitch) = 0;
  virtual void set_position(const Vector& position) = 0;
  virtual void set_velocity(const Vector& velocity) = 0;
  /** Place this in the world. It holds its level nearby and reaches silence
      at the distance the engine stops running the object that made it. */
  virtual void set_placed_range() = 0;
  /** Place this as a sound only meant to be heard from nearby, a ticking fuse
      or a flame. It carries a short way and is silent well inside the view.
      level scales it against other close sounds. Both of these settle the
      gain, so call them before anything that sets it, not after. */
  virtual void set_close_range(float level = 1.0f) = 0;

private:
  SoundSource(const SoundSource&) = delete;
  SoundSource& operator=(const SoundSource&) = delete;
};

#endif

/* EOF */
