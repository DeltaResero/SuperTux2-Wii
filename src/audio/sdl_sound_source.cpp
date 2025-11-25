//  src/audio/sdl_sound_source.cpp
//
//  SuperTux
//  Copyright (C) 2025 DeltaResero
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 2
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

#include "audio/sdl_sound_source.hpp"

#ifdef USE_SDL_MIXER

SDLSoundSource::SDLSoundSource(Mix_Chunk* chunk_) :
  chunk(chunk_),
  channel(-1),
  looping(false),
  gain(1.0f)
{
}

SDLSoundSource::~SDLSoundSource()
{
  stop();
}

void
SDLSoundSource::play()
{
  channel = Mix_PlayChannel(-1, chunk, looping ? -1 : 0);
  if(channel != -1) {
    Mix_Volume(channel, static_cast<int>(gain * MIX_MAX_VOLUME));
  }
}

void
SDLSoundSource::stop()
{
  if(channel != -1) {
    Mix_HaltChannel(channel);
    channel = -1;
  }
}

bool
SDLSoundSource::playing() const
{
  if(channel == -1)
    return false;
  return Mix_Playing(channel) != 0;
}

void
SDLSoundSource::set_looping(bool looping_)
{
  this->looping = looping_;
}

void
SDLSoundSource::set_relative(bool /*relative*/)
{
  // Stub: SDL_mixer doesn't easily support relative/absolute switching per channel
}

void
SDLSoundSource::set_gain(float gain_)
{
  this->gain = gain_;
  if(channel != -1) {
    Mix_Volume(channel, static_cast<int>(gain * MIX_MAX_VOLUME));
  }
}

void
SDLSoundSource::set_pitch(float /*pitch*/)
{
  // Stub
}

void
SDLSoundSource::set_position(const Vector& /*position*/)
{
  // Stub
}

void
SDLSoundSource::set_velocity(const Vector& /*velocity*/)
{
  // Stub
}

void
SDLSoundSource::set_reference_distance(float /*distance*/)
{
  // Stub
}

#endif // USE_SDL_MIXER

/* EOF */
