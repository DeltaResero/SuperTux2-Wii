// src/audio/openal_sound_source.cpp
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

#include "audio/openal_sound_source.hpp"

#include "audio/openal_device.hpp"
#include "audio/sound_manager.hpp"

OpenALSoundSource::OpenALSoundSource() :
  source()
{
  alGenSources(1, &source);
  OpenALDevice::check_al_error("Couldn't create audio source: ");

  /* Give every source the ordinary fall off up front. A source that is never
     placed sits on the listener, where the fall off leaves it alone, so this
     costs nothing there. Without it a placed source that asks for neither
     range would carry to the end of the level at full volume, which the
     linear model does when it is handed no silence distance. */
  alSourcef(source, AL_REFERENCE_DISTANCE, SoundManager::listener_setback());
  alSourcef(source, AL_MAX_DISTANCE, SoundManager::placed_silence());
}

OpenALSoundSource::~OpenALSoundSource()
{
  stop();
  alDeleteSources(1, &source);
}

void
OpenALSoundSource::stop()
{
  alSourceRewindv(1, &source); // Stops the source
  alSourcei(source, AL_BUFFER, AL_NONE);
  OpenALDevice::check_al_error("Problem stopping audio source: ");
}

void
OpenALSoundSource::play()
{
  alSourcePlay(source);
  OpenALDevice::check_al_error("Couldn't start audio source: ");
}

bool
OpenALSoundSource::playing() const
{
  ALint state = AL_PLAYING;
  alGetSourcei(source, AL_SOURCE_STATE, &state);
  return state != AL_STOPPED;
}

void
OpenALSoundSource::pause()
{
  alSourcePause(source);
  OpenALDevice::check_al_error("Couldn't pause audio source: ");
}

void
OpenALSoundSource::resume()
{
  if( !this->paused() )
  {
    return;
  }

  this->play();
}

bool
OpenALSoundSource::paused() const
{
    ALint state = AL_PAUSED;
    alGetSourcei(source, AL_SOURCE_STATE, &state);
    return state == AL_PAUSED;
}

void
OpenALSoundSource::update()
{
}

void
OpenALSoundSource::set_looping(bool looping)
{
  alSourcei(source, AL_LOOPING, looping ? AL_TRUE : AL_FALSE);
}

void
OpenALSoundSource::set_relative(bool relative)
{
  alSourcei(source, AL_SOURCE_RELATIVE, relative ? AL_TRUE : AL_FALSE);
}

void
OpenALSoundSource::set_position(const Vector& position)
{
  alSource3f(source, AL_POSITION, position.x, position.y, 0);
}

void
OpenALSoundSource::set_velocity(const Vector& velocity)
{
  alSource3f(source, AL_VELOCITY, velocity.x, velocity.y, 0);
}

void
OpenALSoundSource::set_gain(float gain)
{
  alSourcef(source, AL_GAIN, gain);
}

void
OpenALSoundSource::set_pitch(float pitch)
{
  alSourcef(source, AL_PITCH, pitch);
}

void
OpenALSoundSource::set_placed_range()
{
  alSourcef(source, AL_GAIN, SoundManager::placed_level());
  alSourcef(source, AL_REFERENCE_DISTANCE, SoundManager::listener_setback());
  alSourcef(source, AL_MAX_DISTANCE, SoundManager::placed_silence());
}

void
OpenALSoundSource::set_close_range(float level)
{
  /* Holds its level out as far as the listener stands, so standing on one
     gives the whole close level rather than a fraction of it, then falls to
     nothing over the distance it is meant to carry. */
  alSourcef(source, AL_GAIN, SoundManager::close_level() * level);
  alSourcef(source, AL_REFERENCE_DISTANCE, SoundManager::listener_setback());
  alSourcef(source, AL_MAX_DISTANCE, SoundManager::close_silence());
}

/* EOF */
