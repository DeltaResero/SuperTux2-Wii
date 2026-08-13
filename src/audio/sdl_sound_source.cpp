// src/audio/sdl_sound_source.cpp
// SPDX-License-Identifier: GPL-3.0-or-later
//
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

#include "audio/sdl_sound_source.hpp"

#include <algorithm>
#include <cmath>
#include <numbers>

#include "audio/sdl_mixer_device.hpp"
#include "audio/sound_manager.hpp"

SDLSoundSource::SDLSoundSource(SDLMixerDevice& device, Mix_Chunk* chunk) :
  m_device(device),
  m_chunk(chunk),
  m_channel(-1),
  m_looping(false),
  m_gain(1.0f),
  m_placement(UNPLACED),
  m_level(1.0f),
  m_position(0.0f, 0.0f),
  m_positioned(false)
{
}

SDLSoundSource::~SDLSoundSource()
{
  stop();
}

bool
SDLSoundSource::holds_channel() const
{
  /* SDL_mixer hands a channel back for reuse the moment a sound ends, so a
     channel number on its own means nothing. The chunk still sitting on it
     is what says the channel is ours. */
  return m_channel >= 0 && Mix_GetChunk(m_channel) == m_chunk;
}

void
SDLSoundSource::play()
{
  m_channel = Mix_PlayChannel(-1, m_chunk, m_looping ? -1 : 0);
  if(m_channel < 0)
    return;   // every channel busy; the sound is simply dropped

  apply_placement();
}

void
SDLSoundSource::stop()
{
  if(holds_channel())
    Mix_HaltChannel(m_channel);
  m_channel = -1;
}

bool
SDLSoundSource::playing() const
{
  /* Counts a paused sound as still going, matching the OpenAL source, since
     the manager drops a source the moment this turns false. */
  return holds_channel() && Mix_Playing(m_channel) != 0;
}

void
SDLSoundSource::pause()
{
  if(holds_channel())
    Mix_Pause(m_channel);
}

void
SDLSoundSource::resume()
{
  if(holds_channel())
    Mix_Resume(m_channel);
}

bool
SDLSoundSource::paused() const
{
  return holds_channel() && Mix_Paused(m_channel) != 0;
}

void
SDLSoundSource::update()
{
  // SDL_mixer refills its own channels; nothing to do between frames.
}

void
SDLSoundSource::set_looping(bool looping)
{
  m_looping = looping;
}

void
SDLSoundSource::set_relative(bool relative)
{
  /* Relative means the sound sits on the listener, which is how an unplaced
     sound is asked for. */
  if(relative)
  {
    m_placement = UNPLACED;
    m_positioned = false;
    apply_placement();
  }
}

void
SDLSoundSource::set_gain(float gain)
{
  m_gain = gain;
  apply_placement();
}

void
SDLSoundSource::set_pitch(float)
{
  /* SDL_mixer plays a chunk at the rate it was mixed at and offers no way to
     shift it. Nothing in the game asks for this. */
}

void
SDLSoundSource::set_position(const Vector& position)
{
  m_position = position;
  m_positioned = true;
  apply_placement();
}

void
SDLSoundSource::set_velocity(const Vector&)
{
  /* Would be for a doppler shift, which needs the pitch control SDL_mixer
     does not have. Nothing in the game asks for this either. */
}

void
SDLSoundSource::set_placed_range()
{
  m_placement = PLACED;
  m_level = SoundManager::placed_level();
  apply_placement();
}

void
SDLSoundSource::set_close_range(float level)
{
  m_placement = CLOSE;
  m_level = SoundManager::close_level() * level;
  apply_placement();
}

void
SDLSoundSource::apply_placement()
{
  if(!holds_channel())
    return;

  const float level = std::clamp(m_gain * m_level, 0.0f, 1.0f);
  Mix_Volume(m_channel, static_cast<int>(level * MIX_MAX_VOLUME));

  if(m_placement == UNPLACED || !m_positioned)
  {
    Mix_SetPosition(m_channel, 0, 0);
    return;
  }

  const float setback = SoundManager::listener_setback();
  const Vector listener = m_device.get_listener_position();
  const float dx = m_position.x - listener.x;
  const float dy = m_position.y - listener.y;

  /* The listener stands back from the plane the game is drawn on, so the
     distance has three parts even though the world is flat. */
  const float distance = std::sqrt(dx * dx + dy * dy + setback * setback);

  const float silence = (m_placement == CLOSE) ? SoundManager::close_silence()
                                               : SoundManager::placed_silence();

  /* The same straight line the OpenAL device is set up to draw: full volume
     out to the setback, nothing left by the silence distance. */
  float carried = 1.0f;
  if(silence > setback)
    carried = 1.0f - (distance - setback) / (silence - setback);
  carried = std::clamp(carried, 0.0f, 1.0f);

  /* SDL_mixer's distance is a plain attenuation, nothing at 0 and silent at
     255, so whatever fall off we want maps straight onto it and we are not
     stuck with the shape it would have chosen. */
  const Uint8 attenuation = static_cast<Uint8>((1.0f - carried) * 255.0f + 0.5f);

  /* Angle zero is straight ahead and ninety is to the right. Sideways offset
     is the only part that pans, and the setback is what keeps a sound beside
     Tux from going hard into one ear. */
  int degrees = static_cast<int>(std::atan2(dx, setback)
                                 * 180.0f / std::numbers::pi_v<float>);
  if(degrees < 0)
    degrees += 360;

  Mix_SetPosition(m_channel, static_cast<Sint16>(degrees), attenuation);
}

/* EOF */
