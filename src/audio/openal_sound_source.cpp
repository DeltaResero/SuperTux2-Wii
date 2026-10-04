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

#include <algorithm>
#include <cmath>

#include "audio/sound_manager.hpp"
#include "util/log.hpp"

namespace {

// OpenAL Soft sends a mono source at angle a to each ear as 0.5 +- 0.5 sin(t) + PAN_FRONT cos(t), with t = a * PAN_STRETCH
const float PAN_FRONT = 0.0956f;
const float PAN_STRETCH = 1.5f;

// A quarter of a dB as a fraction, a change in one ear too small to hear
const float HEARD_STEP = 0.029f;

bool sounds_the_same(float level, float sent)
{
  return std::abs(level - sent) <= sent * HEARD_STEP;
}

} // namespace

OpenALSoundSource::OpenALSoundSource() :
  m_source(),
  m_gain(1.0f),
  m_volume(1.0f),
  m_placement(Placement::NONE),
  m_close_range(CLOSE_RANGE),
  m_position(),
  m_positioned(false),
  m_full(false),
  m_sent_left(-1.0f),
  m_sent_right(-1.0f)
{
  alGenSources(1, &m_source);

  // Don't catch anything here: force the caller to catch the error, so that
  // the caller won't handle an object in an invalid state thinking it's clean
  SoundManager::check_al_error("Couldn't create audio source: ");

  alSourcef(m_source, AL_REFERENCE_DISTANCE, 128);
}

OpenALSoundSource::~OpenALSoundSource()
{
  stop();
  alDeleteSources(1, &m_source);
}

void
OpenALSoundSource::stop()
{
#ifdef WIN32
  // See commit 417a8e7a8c599bfc2dceaec7b6f64ac865318ef1
  alSourceRewindv(1, &m_source); // Stops the source
#else
  alSourceStop(m_source);
#endif
  alSourcei(m_source, AL_BUFFER, AL_NONE);
  try
  {
    SoundManager::check_al_error("Problem stopping audio source: ");
  }
  catch(const std::exception& e)
  {
    // Internal OpenAL error. Don't you crash on me, baby!
    log_warning << e.what() << std::endl;
  }
}

void
OpenALSoundSource::play()
{
  alSourcePlay(m_source);

  try
  {
    SoundManager::check_al_error("Couldn't start audio source: ");
  }
  catch(const std::exception& e)
  {
    // We probably have too many sources playing simultaneously.
    log_warning << e.what() << std::endl;
  }
}

bool
OpenALSoundSource::playing() const
{
  ALint state = AL_PLAYING;
  alGetSourcei(m_source, AL_SOURCE_STATE, &state);
  return state == AL_PLAYING;
}

void
OpenALSoundSource::pause()
{
  alSourcePause(m_source);
  try
  {
    SoundManager::check_al_error("Couldn't pause audio source: ");
  }
  catch(const std::exception& e)
  {
    log_warning << e.what() << std::endl;
  }
}

void
OpenALSoundSource::resume()
{
  if ( !paused() )
  {
    return;
  }

  play();
}

bool
OpenALSoundSource::paused() const
{
    ALint state = AL_PAUSED;
    alGetSourcei(m_source, AL_SOURCE_STATE, &state);
    return state == AL_PAUSED;
}

void
OpenALSoundSource::update()
{
}

void
OpenALSoundSource::set_looping(bool looping)
{
  alSourcei(m_source, AL_LOOPING, looping ? AL_TRUE : AL_FALSE);
}

void
OpenALSoundSource::set_relative(bool relative)
{
  alSourcei(m_source, AL_SOURCE_RELATIVE, relative ? AL_TRUE : AL_FALSE);
  m_sent_left = -1.0f;
}

void
OpenALSoundSource::set_position(const Vector& position)
{
  m_position = position;
  m_positioned = true;
  if (m_placement == Placement::NONE)
    alSource3f(m_source, AL_POSITION, position.x, position.y, 0);
  else
    apply_placement();
}

void
OpenALSoundSource::set_velocity(const Vector& velocity)
{
  alSource3f(m_source, AL_VELOCITY, velocity.x, velocity.y, 0);
}

void
OpenALSoundSource::set_gain(float gain)
{
  m_gain = gain;
  apply_placement();
}

void
OpenALSoundSource::set_pitch(float pitch)
{
  alSourcef(m_source, AL_PITCH, pitch);
}

void
OpenALSoundSource::set_placed_range()
{
  m_placement = Placement::PLACED;
  // The gain can go past 1 to make up for OpenAL's own pan holding a centred sound down
  alSourcef(m_source, AL_MAX_GAIN, 2.0f);
  apply_placement();
}

void
OpenALSoundSource::set_close_range(float range)
{
  m_placement = Placement::CLOSE;
  m_close_range = range;
  alSourcef(m_source, AL_MAX_GAIN, 2.0f);
  apply_placement();
}

void
OpenALSoundSource::set_lean_only()
{
  m_placement = Placement::LEAN;
  alSourcef(m_source, AL_MAX_GAIN, 2.0f);
  apply_placement();
}

void
OpenALSoundSource::set_volume(float volume)
{
  m_volume = volume;
  apply_placement();
}

void
OpenALSoundSource::apply_placement()
{
  if (m_placement == Placement::NONE || !m_positioned) {
    alSourcef(m_source, AL_GAIN, m_gain * m_volume);
    m_sent_left = -1.0f;
    return;
  }

  float left, right;
  if (m_placement == Placement::LEAN)
    SoundManager::current()->get_lean(m_position, left, right);
  else
    SoundManager::current()->get_placement(m_position, m_placement == Placement::CLOSE, m_close_range, m_full, left, right);
  const float left_ear = std::min(left * m_gain, 1.0f) * m_volume;
  const float right_ear = std::min(right * m_gain, 1.0f) * m_volume;

  // Leaves OpenAL alone until an ear changes by enough to hear
  if (sounds_the_same(left_ear, m_sent_left) && sounds_the_same(right_ear, m_sent_right))
    return;
  m_sent_left = left_ear;
  m_sent_right = right_ear;

  const float near_ear = std::max(left_ear, right_ear);
  const float far_ear = std::min(left_ear, right_ear);

  // Turns the source until OpenAL's pan gives this far to near balance, then sets the gain for the near ear
  const float balance = (near_ear > 0.0f) ? (near_ear - far_ear) / (near_ear + far_ear) : 0.0f;
  const float lift = 2.0f * PAN_FRONT * balance;
  const float turn = std::atan(lift) + std::asin(balance / std::sqrt(1.0f + lift * lift));
  const float angle = std::copysign(turn / PAN_STRETCH, right_ear - left_ear);

  alSourcei(m_source, AL_SOURCE_RELATIVE, AL_TRUE);
  alSource3f(m_source, AL_POSITION, std::sin(angle), 0.0f, -std::cos(angle));
  alSourcef(m_source, AL_GAIN, near_ear / (0.5f + 0.5f * std::sin(turn) + PAN_FRONT * std::cos(turn)));
}

/* EOF */
