// src/audio/sdl_mixer_device.hpp
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

#ifndef HEADER_SUPERTUX_AUDIO_SDL_MIXER_DEVICE_HPP
#define HEADER_SUPERTUX_AUDIO_SDL_MIXER_DEVICE_HPP

#include <map>
#include <string>

#include <SDL_mixer.h>

#include "audio/audio_device.hpp"
#include "math/vector.hpp"

/**
 * SDL_mixer standing in for OpenAL.
 *
 * The Wii has no OpenAL worth using, so this is what the port runs on, and
 * it is buildable on the desktop so that the two can be heard side by side.
 *
 * The differences worth knowing. SDL_mixer has no world, so a source works
 * out its own angle and attenuation from the figures the sound manager
 * holds, which is what keeps the two backends fading a sound over the same
 * distances. It also has no pitch control and no doppler, neither of which
 * anything in the game asks for.
 */
class SDLMixerDevice final : public AudioDevice
{
public:
  SDLMixerDevice();
  ~SDLMixerDevice() override;

  bool is_open() const override { return m_open; }

  std::unique_ptr<SoundSource> create_source(const std::string& filename) override;
  void preload(const std::string& filename) override;

  void play_music(const std::string& filename, float fade_in) override;
  void stop_music(float fade_out) override;
  void pause_music(float fade_out) override;
  void resume_music(float fade_in) override;
  bool has_music() const override { return m_music != nullptr; }

  void set_listener_position(const Vector& position) override;
  void set_listener_velocity(const Vector& velocity) override;
  void set_listener_orientation(const Vector& at, const Vector& up) override;

  void update() override;

  /** Where the listener stands. A source asks on its way to working out how
      far away and how far to one side it is. */
  const Vector& get_listener_position() const { return m_listener; }

private:
  /** The chunk for this file, reading it if it is not held yet. Throws if it
      cannot be read. Ownership stays here. */
  Mix_Chunk* chunk_for(const std::string& filename);
  void release_music();
  /** Jump back to the loop point if the track has run past where it should
      repeat. Does nothing for a track that simply loops whole. */
  void follow_loop_point();

  bool m_open;

  /** Every sound already read, by name, held for as long as the device is. */
  std::map<std::string, Mix_Chunk*> m_chunks;

  Mix_Music* m_music;
  /** Second to jump back to, and the second to jump at. A jump second of
      zero means the track loops whole and needs no watching. */
  float m_loop_begin;
  float m_loop_jump_at;

  Vector m_listener;

private:
  SDLMixerDevice(const SDLMixerDevice&) = delete;
  SDLMixerDevice& operator=(const SDLMixerDevice&) = delete;
};

#endif

/* EOF */
