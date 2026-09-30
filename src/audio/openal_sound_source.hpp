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

#ifndef HEADER_SUPERTUX_AUDIO_OPENAL_SOUND_SOURCE_HPP
#define HEADER_SUPERTUX_AUDIO_OPENAL_SOUND_SOURCE_HPP

#include <al.h>

#include "audio/sound_source.hpp"
#include "math/vector.hpp"

class OpenALSoundSource : public SoundSource
{
  friend class SoundManager;

public:
  OpenALSoundSource();
  ~OpenALSoundSource() override;

  virtual void play() override;
  virtual void stop() override;
  virtual void pause() override;
  virtual bool playing() const override;

  virtual void set_looping(bool looping) override;
  virtual void set_relative(bool relative) override;
  virtual void set_gain(float gain) override;
  virtual void set_pitch(float pitch) override;
  virtual void set_position(const Vector& position) override;
  virtual void set_velocity(const Vector& position) override;
  virtual void set_placed_range() override;
  virtual void set_close_range(float range) override;

  virtual void set_volume(float volume);

  virtual bool paused() const;
  virtual void resume();
  virtual void update();

private:
  enum class Placement { NONE, PLACED, CLOSE };

  /** Sets both ears' volume from SoundManager::get_placement, where OpenAL alone pans much harder */
  void apply_placement();

protected:
  ALuint m_source;
  float m_gain;
  float m_volume;

private:
  Placement m_placement;
  /** How far a close sound carries */
  float m_close_range;
  Vector m_position;
  bool m_positioned;
  /** A sound vanilla played at full volume, having been a stereo file */
  bool m_full;
  /** Each ear's level OpenAL was last given, below zero before the first */
  float m_sent_left;
  float m_sent_right;

private:
  OpenALSoundSource(const OpenALSoundSource&) = delete;
  OpenALSoundSource& operator=(const OpenALSoundSource&) = delete;
};

#endif

/* EOF */
