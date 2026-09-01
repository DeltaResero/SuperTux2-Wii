// src/audio/openal_device.cpp
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

#include "audio/openal_device.hpp"

#include <memory>
#include <sstream>
#include <stdexcept>
#include <exception>
#include <utility>

#include "audio/openal_sound_source.hpp"
#include "audio/sound_file.hpp"
#include "audio/sound_manager.hpp"
#include "audio/stream_sound_source.hpp"
#include "math/vector.hpp"
#include "util/log.hpp"

namespace {

/** A sound larger than this is streamed from disk a fragment at a time
    rather than held whole. */
const size_t STREAM_ABOVE = 100000;

} // namespace

OpenALDevice::OpenALDevice() :
  m_device(nullptr),
  m_context(nullptr),
  m_music(),
  m_buffers()
{
  try {
    m_device = alcOpenDevice(0);
    if (m_device == nullptr) {
      throw std::runtime_error("Couldn't open audio device.");
    }

    int attributes[] = { 0 };
    m_context = alcCreateContext(m_device, attributes);
    check_alc_error("Couldn't create audio context: ");
    alcMakeContextCurrent(m_context);
    check_alc_error("Couldn't select audio context: ");

    alDistanceModel(AL_LINEAR_DISTANCE_CLAMPED);
    check_al_error("Audio error after init: ");
  } catch(std::exception& e) {
    if(m_context != nullptr) {
      alcDestroyContext(m_context);
      m_context = nullptr;
    }
    if(m_device != nullptr) {
      alcCloseDevice(m_device);
      m_device = nullptr;
    }
    log_warning << "Couldn't initialize audio device: " << e.what() << std::endl;
    print_version();
  }
}

OpenALDevice::~OpenALDevice()
{
  /* The music streams through buffers of its own, so let it go before the
     context that owns them. */
  m_music.reset();

  for(const auto& buffer : m_buffers) {
    alDeleteBuffers(1, &buffer.second);
  }

  if(m_context != nullptr) {
    alcDestroyContext(m_context);
    m_context = nullptr;
  }
  if(m_device != nullptr) {
    alcCloseDevice(m_device);
    m_device = nullptr;
  }
}

ALuint
OpenALDevice::read_into_buffer(SoundFile& file)
{
  ALenum format = sample_format(file);
  ALuint buffer;
  alGenBuffers(1, &buffer);
  check_al_error("Couldn't create audio buffer: ");
  std::unique_ptr<char[]> samples(new char[file.size]);
  file.read(samples.get(), file.size);
  alBufferData(buffer, format, samples.get(),
               static_cast<ALsizei>(file.size),
               static_cast<ALsizei>(file.rate));
  check_al_error("Couldn't fill audio buffer: ");

  return buffer;
}

std::unique_ptr<SoundSource>
OpenALDevice::create_source(const std::string& filename)
{
  std::unique_ptr<OpenALSoundSource> source(new OpenALSoundSource);

  ALuint buffer;

  // reuse an existing static sound buffer
  SoundBuffers::iterator i = m_buffers.find(filename);
  if(i != m_buffers.end()) {
    buffer = i->second;
  } else {
    std::unique_ptr<SoundFile> file(load_sound_file(filename));

    if(file->size < STREAM_ABOVE) {
      buffer = read_into_buffer(*file);
      m_buffers.insert(std::make_pair(filename, buffer));
    } else {
      std::unique_ptr<StreamSoundSource> streamed(new StreamSoundSource);
      streamed->set_sound_file(std::move(file));
      return streamed;
    }

    log_debug << "Uncached sound \"" << filename << "\" requested to be played" << std::endl;
  }

  alSourcei(source->source, AL_BUFFER, buffer);
  return source;
}

void
OpenALDevice::preload(const std::string& filename)
{
  if(m_buffers.find(filename) != m_buffers.end())
    return;

  try {
    std::unique_ptr<SoundFile> file(load_sound_file(filename));
    // only keep small files
    if(file->size >= STREAM_ABOVE)
      return;

    m_buffers.insert(std::make_pair(filename, read_into_buffer(*file)));
  } catch(std::exception& e) {
    log_warning << "Error while preloading sound file: " << e.what() << std::endl;
  }
}

void
OpenALDevice::play_music(const std::string& filename, float fade_in)
{
  std::unique_ptr<StreamSoundSource> track(new StreamSoundSource);
  track->set_sound_file(load_sound_file(filename));
  track->set_looping(true);
  /* Relative to the listener, so the fall off that places everything else
     never touches it. */
  track->set_relative(true);
  if(fade_in > 0.0f)
    track->set_fading(StreamSoundSource::FadingOn, fade_in);
  track->play();

  m_music = std::move(track);
}

void
OpenALDevice::stop_music(float fade_out)
{
  if(fade_out > 0.0f) {
    if(m_music && m_music->get_fade_state() != StreamSoundSource::FadingOff)
      m_music->set_fading(StreamSoundSource::FadingOff, fade_out);
  } else {
    m_music.reset();
  }
}

void
OpenALDevice::pause_music(float fade_out)
{
  if(!m_music)
    return;

  if(fade_out > 0.0f) {
    if(m_music->get_fade_state() != StreamSoundSource::FadingPause)
      m_music->set_fading(StreamSoundSource::FadingPause, fade_out);
  } else {
    m_music->pause();
  }
}

void
OpenALDevice::resume_music(float fade_in)
{
  if(!m_music)
    return;

  if(fade_in > 0.0f) {
    if(m_music->get_fade_state() != StreamSoundSource::FadingResume) {
      /* A fade to a pause leaves the source paused once it runs out, and
         raising the gain of a paused source is still silence, so it has to be
         playing again before the fade is given anything to do. */
      m_music->resume();
      m_music->set_fading(StreamSoundSource::FadingResume, fade_in);
    }
  } else if(m_music->paused()) {
    m_music->resume();
  } else if(!m_music->playing()) {
    m_music->play();
  }
}

void
OpenALDevice::set_listener_position(const Vector& pos)
{
  alListener3f(AL_POSITION, pos.x, pos.y, -SoundManager::listener_setback());
}

void
OpenALDevice::set_listener_velocity(const Vector& vel)
{
  alListener3f(AL_VELOCITY, vel.x, vel.y, 0);
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
  if (m_music)
    m_music->update();

  if (m_context)
  {
    alcProcessContext(m_context);
    check_alc_error("Error while processing audio context: ");
  }
}

ALenum
OpenALDevice::sample_format(const SoundFile& file)
{
  if(file.channels == 2) {
    if(file.bits_per_sample == 16) {
      return AL_FORMAT_STEREO16;
    } else if(file.bits_per_sample == 8) {
      return AL_FORMAT_STEREO8;
    } else {
      throw std::runtime_error("Only 16 and 8 bit samples supported");
    }
  } else if(file.channels == 1) {
    if(file.bits_per_sample == 16) {
      return AL_FORMAT_MONO16;
    } else if(file.bits_per_sample == 8) {
      return AL_FORMAT_MONO8;
    } else {
      throw std::runtime_error("Only 16 and 8 bit samples supported");
    }
  }

  throw std::runtime_error("Only 1 and 2 channel samples supported");
}

void
OpenALDevice::print_version()
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
  if(err != ALC_NO_ERROR) {
    std::stringstream msg;
    msg << message << alcGetString(m_device, err);
    throw std::runtime_error(msg.str());
  }
}

void
OpenALDevice::check_al_error(const char* message)
{
  int err = alGetError();
  if(err != AL_NO_ERROR) {
    std::stringstream msg;
    msg << message << alGetString(err);
    throw std::runtime_error(msg.str());
  }
}

/* EOF */
