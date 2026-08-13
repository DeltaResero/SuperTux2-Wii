// src/audio/audio_device.hpp
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

#ifndef HEADER_SUPERTUX_AUDIO_AUDIO_DEVICE_HPP
#define HEADER_SUPERTUX_AUDIO_AUDIO_DEVICE_HPP

#include <memory>
#include <string>

class SoundSource;
class Vector;

/**
 * The sound hardware, and everything that knows how to talk to it. The sound
 * manager above keeps the parts that do not care which library is underneath:
 * which music is playing, which sources are still alive, and how loudly a
 * thing at a given distance should be heard.
 *
 * A device is only asked for work when a sound begins or the listener moves,
 * never once per sample, so the call through here costs nothing worth
 * counting even on the slower hardware this fork is aimed at.
 */
class AudioDevice
{
public:
  virtual ~AudioDevice() {}

  /** Whether the hardware opened. A device that answers false is left in
      place and simply never asked for anything else. */
  virtual bool is_open() const = 0;

  /** A source playing the named file. Throws if the file cannot be read. */
  virtual std::unique_ptr<SoundSource> create_source(const std::string& filename) = 0;

  /** Read a file in now so that the first play of it does not stall. Quietly
      does nothing for a file too large to be worth holding. */
  virtual void preload(const std::string& filename) = 0;

  virtual void set_listener_position(const Vector& position) = 0;
  virtual void set_listener_velocity(const Vector& velocity) = 0;
  virtual void set_listener_orientation(const Vector& at, const Vector& up) = 0;

  /** Housekeeping the library wants between frames. */
  virtual void update() = 0;

private:
  AudioDevice(const AudioDevice&) = delete;
  AudioDevice& operator=(const AudioDevice&) = delete;

protected:
  AudioDevice() {}
};

#endif

/* EOF */
