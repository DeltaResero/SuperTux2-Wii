// src/audio/sdl_sound_source.hpp
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

#ifndef HEADER_SUPERTUX_AUDIO_SDL_SOUND_SOURCE_HPP
#define HEADER_SUPERTUX_AUDIO_SDL_SOUND_SOURCE_HPP

#include <SDL_mixer.h>

#include "audio/sound_source.hpp"
#include "math/vector.hpp"

class SDLMixerDevice;

/**
 * One sound on one mixing channel.
 *
 * SDL_mixer has no notion of a world, so where OpenAL is told a position and
 * works out the rest, this works out the rest itself and tells SDL_mixer an
 * angle and an attenuation. The figures it works from are the ones the sound
 * manager holds, so both libraries fade a sound over the same distances.
 *
 * A channel is only held while the sound is running. SDL_mixer picks one on
 * play and takes it back when the sound ends, so the channel number here is
 * checked against the chunk still on it before being trusted.
 */
class SDLSoundSource final : public SoundSource
{
public:
  /** The chunk stays owned by the device that cached it. */
  SDLSoundSource(SDLMixerDevice& device, Mix_Chunk* chunk);
  ~SDLSoundSource() override;

  void play() override;
  void stop() override;
  bool playing() const override;
  void pause() override;
  void resume() override;
  bool paused() const override;
  void update() override;

  void set_looping(bool looping) override;
  void set_relative(bool relative) override;
  void set_gain(float gain) override;
  void set_pitch(float pitch) override;
  void set_position(const Vector& position) override;
  void set_velocity(const Vector& velocity) override;

  void set_placed_range() override;
  void set_close_range(float level = 1.0f) override;

private:
  /** Work the stored position into an angle and an attenuation and hand
      them to SDL_mixer. Does nothing while the sound is not on a channel. */
  void apply_placement();
  /** Whether the channel we were given is still ours. */
  bool holds_channel() const;

  /** How a sound is faded with distance. Unplaced sounds are things that
      happen to the player rather than in the world, and are never faded. */
  enum Placement { UNPLACED, PLACED, CLOSE };

  SDLMixerDevice& m_device;
  Mix_Chunk* m_chunk;
  int m_channel;

  bool m_looping;
  float m_gain;
  Placement m_placement;
  /** Scales this against other sounds of the same placement. */
  float m_level;
  Vector m_position;
  bool m_positioned;

private:
  SDLSoundSource(const SDLSoundSource&) = delete;
  SDLSoundSource& operator=(const SDLSoundSource&) = delete;
};

#endif

/* EOF */
