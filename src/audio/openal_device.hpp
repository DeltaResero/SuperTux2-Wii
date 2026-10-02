//  SuperTux
//  Copyright (C) 2006 Matthias Braun <matze@braunis.de>
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

#ifndef HEADER_SUPERTUX_AUDIO_OPENAL_DEVICE_HPP
#define HEADER_SUPERTUX_AUDIO_OPENAL_DEVICE_HPP

#include <map>
#include <string>

#include <al.h>
#include <alc.h>

#include "audio/audio_device.hpp"

class SoundFile;

/** OpenAL itself, with the buffer of every short sound already read */
class OpenALDevice final : public AudioDevice
{
public:
  OpenALDevice();
  ~OpenALDevice() override;

  bool is_open() const override { return m_device != nullptr && m_context != nullptr; }

  std::unique_ptr<SoundSource> create_source(const std::string& filename, bool full) override;
  void preload(const std::string& filename) override;

  void set_listener_position(const Vector& position) override;
  void set_listener_orientation(const Vector& at, const Vector& up) override;

  void update() override;

  static ALenum get_sample_format(const SoundFile& file);
  static void check_al_error(const char* message);

private:
  static ALuint load_file_into_buffer(SoundFile& file);
  static void print_openal_version();
  void check_alc_error(const char* message) const;

private:
  ALCdevice* m_device;
  ALCcontext* m_context;

  std::map<std::string, ALuint> m_buffers;

private:
  OpenALDevice(const OpenALDevice&) = delete;
  OpenALDevice& operator=(const OpenALDevice&) = delete;
};

#endif

/* EOF */
