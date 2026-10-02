//  SuperTux
//  Copyright (C) 2026 DeltaResero
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

#include "math/fwd.hpp"

class SoundSource;

/** The sound library under SoundManager, which keeps everything that doesn't depend on it */
class AudioDevice
{
public:
  AudioDevice() {}
  virtual ~AudioDevice() {}

  /** Whether the hardware opened, as one that didn't is never asked for anything else */
  virtual bool is_open() const = 0;

  /** A source playing filename, full for a sound vanilla played at full volume. Throws if it can't be read */
  virtual std::unique_ptr<SoundSource> create_source(const std::string& filename, bool full) = 0;

  /** Reads a short sound in now, so the first time it plays doesn't stall */
  virtual void preload(const std::string& filename) = 0;

  virtual void set_listener_position(const Vector& position) = 0;
  virtual void set_listener_orientation(const Vector& at, const Vector& up) = 0;

  /** Whatever the library needs doing between frames */
  virtual void update() = 0;

private:
  AudioDevice(const AudioDevice&) = delete;
  AudioDevice& operator=(const AudioDevice&) = delete;
};

#endif

/* EOF */
