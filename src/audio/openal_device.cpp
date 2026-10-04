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

#include "audio/openal_device.hpp"

#include <memory>
#include <sstream>
#include <stdexcept>

#include "audio/openal_sound_source.hpp"
#include "audio/sound_file.hpp"
#include "audio/stream_sound_source.hpp"
#include "math/vector.hpp"
#include "util/log.hpp"

OpenALDevice::OpenALDevice() :
  m_device(alcOpenDevice(nullptr)),
  m_context(alcCreateContext(m_device, nullptr)),
  m_buffers(),
  m_music_source()
{
  try {
    if (m_device == nullptr) {
      throw std::runtime_error("Couldn't open audio device.");
    }
    check_alc_error("Couldn't create audio context: ");
    alcMakeContextCurrent(m_context);
    check_alc_error("Couldn't select audio context: ");

    check_al_error("Audio error after init: ");
  } catch(std::exception& e) {
    if (m_context != nullptr) {
      alcDestroyContext(m_context);
      m_context = nullptr;
    }
    if (m_device != nullptr) {
      alcCloseDevice(m_device);
      m_device = nullptr;
    }
    log_warning << "Couldn't initialize audio device: " << e.what() << std::endl;
    print_openal_version();
  }
}

OpenALDevice::~OpenALDevice()
{
  // The music streams through buffers of its own, which go before the context
  m_music_source.reset();

  for (const auto& buffer : m_buffers) {
    alDeleteBuffers(1, &buffer.second);
  }

  if (m_context != nullptr) {
    alcDestroyContext(m_context);
    m_context = nullptr;
  }
  if (m_device != nullptr) {
    alcCloseDevice(m_device);
    m_device = nullptr;
  }
}

ALuint
OpenALDevice::load_file_into_buffer(SoundFile& file)
{
  ALenum format = get_sample_format(file);
  ALuint buffer;
  alGenBuffers(1, &buffer);
  check_al_error("Couldn't create audio buffer: ");
  std::unique_ptr<char[]> samples(new char[file.m_size]);
  file.read(samples.get(), file.m_size);
  log_debug << "buffer: " << buffer << "\n"
            << "format: " << format << "\n"
            << "samples: " << samples.get() << "\n"
            << "file size: " << static_cast<ALsizei>(file.m_size) << "\n"
            << "file rate: " << static_cast<ALsizei>(file.m_rate) << "\n";

  alBufferData(buffer, format, samples.get(),
               static_cast<ALsizei>(file.m_size),
               static_cast<ALsizei>(file.m_rate));
  check_al_error("Couldn't fill audio buffer: ");

  return buffer;
}

std::unique_ptr<SoundSource>
OpenALDevice::create_source(const std::string& filename, bool full)
{
  auto source = std::make_unique<OpenALSoundSource>();

  ALuint buffer;

  // reuse an existing static sound buffer
  auto it = m_buffers.find(filename);
  if (it != m_buffers.end()) {
    buffer = it->second;
  } else {
    // Load sound file
    std::unique_ptr<SoundFile> file(load_sound_file(filename));

    if (file->m_size < STREAM_FROM) {
      log_debug << "Adding \"" << filename <<
        "\" into the buffer, file size: " << file->m_size << std::endl;
      buffer = load_file_into_buffer(*file);
      m_buffers.insert(std::make_pair(filename, buffer));
    } else {
      log_debug << "Playing \"" << filename <<
        "\" as StreamSoundSource, file size: " << file->m_size << std::endl;
      auto stream_source = std::make_unique<StreamSoundSource>();
      stream_source->m_full = full;
      stream_source->set_sound_file(std::move(file));
      return stream_source;
    }
  }

  alSourcei(source->m_source, AL_BUFFER, buffer);
  source->m_full = full;
  return source;
}

void
OpenALDevice::preload(const std::string& filename)
{
  auto it = m_buffers.find(filename);
  // already loaded?
  if (it != m_buffers.end())
    return;
  try {
    std::unique_ptr<SoundFile> file (load_sound_file(filename));
    // only keep small files
    if (file->m_size >= STREAM_FROM)
      return;

    ALuint buffer = load_file_into_buffer(*file);
    m_buffers.insert(std::make_pair(filename, buffer));
  } catch(std::exception& e) {
    log_warning << "Error while preloading sound file: " << e.what() << std::endl;
  }
}

void
OpenALDevice::play_music(const std::string& filename, float fadetime, float volume)
{
  auto newmusic = std::make_unique<StreamSoundSource>();
  newmusic->set_sound_file(load_sound_file(filename));
  newmusic->set_looping(true);
  newmusic->set_relative(true);
  newmusic->set_volume(volume);
  if (fadetime > 0)
    newmusic->set_fading(StreamSoundSource::FadingOn, fadetime);
  newmusic->play();

  m_music_source = std::move(newmusic);
}

void
OpenALDevice::keep_music_playing()
{
  if (m_music_source == nullptr)
    return;

  if (m_music_source->paused())
  {
    m_music_source->resume();
  }
  else if (!m_music_source->playing())
  {
    m_music_source->play();
  }
}

void
OpenALDevice::stop_music(float fadetime)
{
  if (fadetime > 0) {
    if (m_music_source
        && m_music_source->get_fade_state() != StreamSoundSource::FadingOff)
      m_music_source->set_fading(StreamSoundSource::FadingOff, fadetime);
  } else {
    m_music_source.reset();
  }
}

void
OpenALDevice::pause_music(float fadetime)
{
  if (m_music_source == nullptr)
    return;

  if (fadetime > 0) {
    if (m_music_source
        && m_music_source->get_fade_state() != StreamSoundSource::FadingPause)
      m_music_source->set_fading(StreamSoundSource::FadingPause, fadetime);
  } else {
    m_music_source->pause();
  }
}

void
OpenALDevice::resume_music(float fadetime)
{
  if (m_music_source == nullptr)
    return;

  if (fadetime > 0) {
    if (m_music_source
        && m_music_source->get_fade_state() != StreamSoundSource::FadingResume) {
      m_music_source->set_fading(StreamSoundSource::FadingResume, fadetime);
      m_music_source->resume();
    }
  } else {
    m_music_source->resume();
  }
}

void
OpenALDevice::set_music_volume(float volume)
{
  if (m_music_source != nullptr) m_music_source->set_volume(volume);
}

void
OpenALDevice::set_listener_position(const Vector& pos)
{
  alListener3f(AL_POSITION, pos.x, pos.y, -300);
}

void
OpenALDevice::set_listener_orientation(const Vector& at, const Vector& up)
{
  ALfloat orientation[]={at.x, at.y, 1.0, up.x, up.y, 0.0};
  alListenerfv(AL_ORIENTATION, orientation);
}

void
OpenALDevice::update()
{
  // check streaming sounds
  if (m_music_source) {
    m_music_source->update();
  }

  if (m_context)
  {
    alcProcessContext(m_context);
    check_alc_error("Error while processing audio context: ");
  }
}

ALenum
OpenALDevice::get_sample_format(const SoundFile& file)
{
  if (file.m_channels == 2) {
    if (file.m_bits_per_sample == 16) {
      return AL_FORMAT_STEREO16;
    } else if (file.m_bits_per_sample == 8) {
      return AL_FORMAT_STEREO8;
    } else {
      throw std::runtime_error("Only 16 and 8 bit samples supported");
    }
  } else if (file.m_channels == 1) {
    if (file.m_bits_per_sample == 16) {
      return AL_FORMAT_MONO16;
    } else if (file.m_bits_per_sample == 8) {
      return AL_FORMAT_MONO8;
    } else {
      throw std::runtime_error("Only 16 and 8 bit samples supported");
    }
  }

  throw std::runtime_error("Only 1 and 2 channel samples supported");
}

void
OpenALDevice::print_openal_version()
{
  log_info << "OpenAL Vendor: " << alGetString(AL_VENDOR) << std::endl;
  log_info << "OpenAL Version: " << alGetString(AL_VERSION) << std::endl;
  log_info << "OpenAL Renderer: " << alGetString(AL_RENDERER) << std::endl;
  log_info << "OpenAl Extensions: " << alGetString(AL_EXTENSIONS) << std::endl;
}

void
OpenALDevice::check_alc_error(const char* message) const
{
  int err = alcGetError(m_device);
  if (err != ALC_NO_ERROR) {
    std::stringstream msg;
    msg << message << alcGetString(m_device, err);
    throw std::runtime_error(msg.str());
  }
}

void
OpenALDevice::check_al_error(const char* message)
{
  int err = alGetError();
  if (err != AL_NO_ERROR) {
    std::stringstream msg;
    msg << message << alGetString(err);
    throw std::runtime_error(msg.str());
  }
}

/* EOF */
