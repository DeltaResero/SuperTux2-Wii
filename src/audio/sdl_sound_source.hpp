//  src/audio/sdl_sound_source.hpp
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

#ifndef HEADER_SUPERTUX_AUDIO_SDL_SOUND_SOURCE_HPP
#define HEADER_SUPERTUX_AUDIO_SDL_SOUND_SOURCE_HPP

#include <config.h>

#ifdef USE_SDL_MIXER

#include <SDL_mixer.h>
#include "audio/sound_source.hpp"
#include "math/vector.hpp"

class SDLSoundSource : public SoundSource
{
public:
  SDLSoundSource(Mix_Chunk* chunk);
  virtual ~SDLSoundSource();

  virtual void play() override;
  virtual void stop() override;
  virtual bool playing() const override;

  virtual void set_looping(bool looping) override;
  virtual void set_relative(bool relative) override; // <--- Added this
  virtual void set_gain(float gain) override;
  virtual void set_pitch(float pitch) override;
  virtual void set_position(const Vector& position) override;
  virtual void set_velocity(const Vector& velocity) override;
  virtual void set_reference_distance(float distance) override;

private:
  Mix_Chunk* chunk;
  int channel;
  bool looping;
  float gain;

private:
  SDLSoundSource(const SDLSoundSource&) = delete;
  SDLSoundSource& operator=(const SDLSoundSource&) = delete;
};

#endif // USE_SDL_MIXER

#endif

/* EOF */
