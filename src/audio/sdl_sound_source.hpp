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

#include "audio/sound_source.hpp"
#include "math/vector.hpp"

struct Mix_Chunk;
class SDLMixerDevice;

/** A sound on an SDL_mixer channel, given each ear's level the OpenAL backend would play it at */
class SDLSoundSource final : public SoundSource
{
public:
  SDLSoundSource(SDLMixerDevice& device, Mix_Chunk* chunk, bool stereo, bool full);
  ~SDLSoundSource() override;

  virtual void play() override;
  virtual void stop() override;
  virtual void pause() override;
  virtual void resume() override;
  virtual bool playing() const override;
  virtual bool paused() const override;
  virtual void update() override;

  virtual void set_looping(bool looping) override;
  virtual void set_relative(bool relative) override;
  virtual void set_gain(float gain) override;
  virtual void set_volume(float volume) override;
  virtual void set_pitch(float pitch) override;
  virtual void set_position(const Vector& position) override;
  virtual void set_velocity(const Vector& velocity) override;
  virtual void set_placed_range() override;
  virtual void set_close_range(float range) override;
  virtual void set_lean_only() override;
  virtual void update_placement() override;

  /** For a sound nothing places, which OpenAL hears from wherever the listener is */
  void listener_moved();

private:
  enum class Placement { NONE, PLACED, CLOSE, LEAN };

  bool holds_channel() const;

  /** Works out each ear's level, returning false when a placed sound hasn't changed enough to hear */
  bool work_out_levels();
  void refresh();
  void send_levels(bool fresh);

private:
  SDLMixerDevice& m_device;
  Mix_Chunk* m_chunk;
  bool m_stereo;
  /** A sound vanilla played at full volume, having been a stereo file */
  bool m_full;

  int m_channel;
  unsigned m_play;
  bool m_looping;
  bool m_relative;
  bool m_stopped;

  float m_gain;
  float m_volume;
  Placement m_placement;
  float m_close_range;
  Vector m_position;
  bool m_positioned;

  /** Each ear's level of a placed sound when last worked out, below zero before the first */
  float m_heard_left;
  float m_heard_right;

  /** Each ear's level, what OpenAL would play */
  float m_left;
  float m_right;

  /** What the channel was last given */
  int m_sent_volume;
  int m_sent_pan_left;
  int m_sent_pan_right;

private:
  SDLSoundSource(const SDLSoundSource&) = delete;
  SDLSoundSource& operator=(const SDLSoundSource&) = delete;
};

#endif

/* EOF */
