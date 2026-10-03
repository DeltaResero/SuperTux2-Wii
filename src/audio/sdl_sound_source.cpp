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

#include <SDL_mixer.h>
#include <algorithm>
#include <cmath>

#include "audio/sdl_mixer_device.hpp"
#include "audio/sdl_stream.hpp"
#include "audio/sdl_voice.hpp"
#include "audio/sound_file.hpp"
#include "audio/sound_manager.hpp"
#include "math/util.hpp"
#include "util/log.hpp"

namespace {

// OpenAL Soft sends a mono source at angle a to each ear as 0.5 +- 0.5 sin(t) + PAN_FRONT cos(t), with t = a * PAN_STRETCH
const float PAN_FRONT = 0.0956f;
const float PAN_STRETCH = 1.5f;

// Where OpenAL hears a sound nothing places from, and how far off it starts to fall away
const float LISTENER_SETBACK = 300.0f;
const float REFERENCE_DISTANCE = 128.0f;

// The most gain OpenAL lets a source have, which a placed one raises
const float MAX_GAIN = 1.0f;
const float PLACED_MAX_GAIN = 2.0f;

// A quarter of a dB as a fraction, a change in one ear too small to hear
const float HEARD_STEP = 0.029f;

bool sounds_the_same(float level, float heard)
{
  return std::abs(level - heard) <= heard * HEARD_STEP;
}

// One ear's share of OpenAL Soft's pan, side 1 for the near ear and -1 for the far one, narrowed by tilt above or below
float pan(float turn, float tilt, float side)
{
  return 0.5f + tilt * (side * 0.5f * std::sin(turn) + PAN_FRONT * std::cos(turn));
}

} // namespace

SDLSoundSource::SDLSoundSource(SDLMixerDevice& device, const std::string& filename, bool stereo, bool full) :
  m_device(device),
  m_filename(filename),
  m_held(true),
  m_file(),
  m_stereo(stereo),
  m_full(full),
  m_channel(-1),
  m_play(0),
  m_pitch(1.0f),
  m_voice(),
  m_stream(),
  m_looping(false),
  m_relative(false),
  m_stopped(false),
  m_gain(1.0f),
  m_volume(1.0f),
  m_placement(Placement::NONE),
  m_close_range(CLOSE_RANGE),
  m_position(0.0f, 0.0f),
  m_positioned(false),
  m_heard_left(-1.0f),
  m_heard_right(-1.0f),
  m_left(0.0f),
  m_right(0.0f),
  m_sent_volume(-1),
  m_sent_pan_left(-1),
  m_sent_pan_right(-1)
{
  m_device.add_source(*this);
  work_out_levels();
}

SDLSoundSource::SDLSoundSource(SDLMixerDevice& device, const std::string& filename, std::unique_ptr<SoundFile> file, bool full) :
  SDLSoundSource(device, filename, file->m_channels == 2, full)
{
  m_held = false;
  m_file = std::move(file);
}

SDLSoundSource::~SDLSoundSource()
{
  stop();
  m_device.remove_source(*this);
}

bool
SDLSoundSource::holds_channel() const
{
  return m_device.carries(m_channel, m_play);
}

bool
SDLSoundSource::over() const
{
  return (m_voice && m_voice->finished()) || (m_stream && m_stream->finished());
}

bool
SDLSoundSource::start_stream()
{
  try {
    // The file's read again for another play
    std::unique_ptr<SoundFile> file = m_file ? std::move(m_file) : load_sound_file(m_filename);
    m_stream = std::make_unique<SDLStream>(std::move(file), m_device.get_rate(), m_looping, SDLMixerDevice::READ_AHEAD);
    return true;
  } catch(std::exception& e) {
    log_warning << "Couldn't play sound " << m_filename << ": " << e.what() << std::endl;
    return false;
  }
}

void
SDLSoundSource::play()
{
  // OpenAL lets go of a stopped source's sound, so it has nothing left to play
  if (m_stopped)
    return;

  if (holds_channel()) {
    // OpenAL carries on with a paused source and starts a playing one over
    if (Mix_Paused(m_channel)) {
      Mix_Resume(m_channel);
      return;
    }
    Mix_HaltChannel(m_channel);
  } else {
    m_channel = m_device.claim_channel(m_play);
    if (m_channel < 0)
      return;
  }

  // The channel plays silence for the sound to be written over, which happens before the panning goes on
  m_voice.reset();
  m_stream.reset();
  if (m_held || m_pitch != 1.0f) {
    const auto& samples = m_device.get_samples(m_filename);
    m_voice = std::make_unique<SDLVoice>(samples.data.data(), samples.data.size() / static_cast<size_t>(samples.channels),
                                         samples.channels, resample_step(samples.rate, m_device.get_rate(), m_pitch),
                                         m_looping);
    Mix_RegisterEffect(m_channel, SDLVoice::feed, nullptr, m_voice.get());
  } else {
    if (!start_stream())
      return;
    Mix_RegisterEffect(m_channel, SDLStream::feed, nullptr, m_stream.get());
  }

  // A channel loses its panning when its last sound ends, so it's always sent again
  send_levels(true);
  Mix_PlayChannel(m_channel, m_device.get_silence(), -1);
}

void
SDLSoundSource::stop()
{
  if (holds_channel())
    Mix_HaltChannel(m_channel);
  m_voice.reset();
  m_stream.reset();
  m_file.reset();
  m_channel = -1;
  m_stopped = true;
}

void
SDLSoundSource::pause()
{
  if (holds_channel())
    Mix_Pause(m_channel);
}

void
SDLSoundSource::resume()
{
  if (!paused())
    return;

  play();
}

bool
SDLSoundSource::playing() const
{
  return holds_channel() && Mix_Paused(m_channel) == 0 && !over();
}

bool
SDLSoundSource::paused() const
{
  return holds_channel() && Mix_Paused(m_channel) != 0;
}

void
SDLSoundSource::update()
{
}

void
SDLSoundSource::set_looping(bool looping)
{
  m_looping = looping;
}

void
SDLSoundSource::set_relative(bool relative)
{
  m_relative = relative;
  m_heard_left = -1.0f;
  refresh();
}

void
SDLSoundSource::set_gain(float gain)
{
  m_gain = gain;
  refresh();
}

void
SDLSoundSource::set_volume(float volume)
{
  m_volume = volume;
  refresh();
}

void
SDLSoundSource::set_pitch(float pitch)
{
  m_pitch = pitch;
  if (m_voice) {
    const auto& samples = m_device.get_samples(m_filename);
    m_voice->set_step(resample_step(samples.rate, m_device.get_rate(), m_pitch));
  }
}

void
SDLSoundSource::set_position(const Vector& position)
{
  m_position = position;
  m_positioned = true;
  refresh();
}

void
SDLSoundSource::set_velocity(const Vector& )
{
  // Only OpenAL's doppler shift used it, which needs a pitch SDL_mixer hasn't got
}

void
SDLSoundSource::set_placed_range()
{
  m_placement = Placement::PLACED;
  refresh();
}

void
SDLSoundSource::set_close_range(float range)
{
  m_placement = Placement::CLOSE;
  m_close_range = range;
  refresh();
}

void
SDLSoundSource::set_lean_only()
{
  m_placement = Placement::LEAN;
  refresh();
}

void
SDLSoundSource::update_placement()
{
  if (m_placement != Placement::NONE)
    refresh();
}

void
SDLSoundSource::keep_up()
{
  if (m_stream)
    m_stream->fill();

  // Halting takes the voice or stream off the channel, so they're safe to keep until the next play
  if (over() && holds_channel())
    Mix_HaltChannel(m_channel);
}

void
SDLSoundSource::listener_moved()
{
  if ((m_placement == Placement::NONE || !m_positioned) && !m_relative && !m_stereo)
    refresh();
}

bool
SDLSoundSource::work_out_levels()
{
  if (m_placement == Placement::NONE || !m_positioned) {
    m_heard_left = -1.0f;
    const float most = (m_placement == Placement::NONE) ? MAX_GAIN : PLACED_MAX_GAIN;
    const float gain = m_gain * m_volume;
    if (m_stereo) {
      // OpenAL never moves or fades a stereo sound
      m_left = m_right = std::min(gain, most);
    } else if (m_relative) {
      m_left = m_right = pan(0.0f, 1.0f, 0.0f) * std::min(gain, most);
    } else {
      // Heard from the listener, falling away past the reference distance and leaning toward its side
      const Vector& listener = m_device.get_listener_position();
      const float dx = m_position.x - listener.x;
      const float dy = m_position.y - listener.y;
      const float level = std::sqrt(dx * dx + LISTENER_SETBACK * LISTENER_SETBACK);
      const float distance = std::sqrt(level * level + dy * dy);
      const float heard = std::min(gain * REFERENCE_DISTANCE / std::max(distance, REFERENCE_DISTANCE), most);
      const float turn = std::min(PAN_STRETCH * std::atan2(std::abs(dx), LISTENER_SETBACK), math::PI_2);
      const float near_ear = heard * pan(turn, level / distance, 1.0f);
      const float far_ear = heard * pan(turn, level / distance, -1.0f);
      m_left = (dx > 0.0f) ? far_ear : near_ear;
      m_right = (dx > 0.0f) ? near_ear : far_ear;
    }
    return true;
  }

  float left, right;
  if (m_placement == Placement::LEAN)
    SoundManager::current()->get_lean(m_position, left, right);
  else
    SoundManager::current()->get_placement(m_position, m_placement == Placement::CLOSE, m_close_range, m_full, left, right);
  const float left_ear = std::min(left * m_gain, 1.0f) * m_volume;
  const float right_ear = std::min(right * m_gain, 1.0f) * m_volume;

  // Holds still until an ear changes by enough to hear, as the OpenAL backend does
  if (sounds_the_same(left_ear, m_heard_left) && sounds_the_same(right_ear, m_heard_right))
    return false;
  m_heard_left = left_ear;
  m_heard_right = right_ear;

  if (m_stereo) {
    // OpenAL turns a placed source to get the balance, but as it never pans a stereo one, only that gain is heard
    const float near_ear = std::max(left_ear, right_ear);
    const float far_ear = std::min(left_ear, right_ear);
    const float balance = (near_ear > 0.0f) ? (near_ear - far_ear) / (near_ear + far_ear) : 0.0f;
    const float lift = 2.0f * PAN_FRONT * balance;
    const float turn = std::atan(lift) + std::asin(balance / std::sqrt(1.0f + lift * lift));
    m_left = m_right = std::min(near_ear / pan(turn, 1.0f, 1.0f), PLACED_MAX_GAIN);
  } else {
    m_left = left_ear;
    m_right = right_ear;
  }
  return true;
}

void
SDLSoundSource::refresh()
{
  if (work_out_levels() && holds_channel())
    send_levels(false);
}

void
SDLSoundSource::send_levels(bool fresh)
{
  // SDL_mixer can't play a sound louder than the file
  const float left = std::clamp(m_left, 0.0f, 1.0f);
  const float right = std::clamp(m_right, 0.0f, 1.0f);

  // The channel volume takes the louder ear and the panning the rest, for the finest steps the two allow
  const int volume = static_cast<int>(std::ceil(std::max(left, right) * MIX_MAX_VOLUME));
  int pan_left = 255;
  int pan_right = 255;
  if (volume > 0) {
    const float scale = 255.0f * MIX_MAX_VOLUME / static_cast<float>(volume);
    pan_left = std::min(static_cast<int>(std::lround(left * scale)), 255);
    pan_right = std::min(static_cast<int>(std::lround(right * scale)), 255);
  }

  // Both wait for the mixer, so they only go when a value changes
  if (fresh || volume != m_sent_volume) {
    Mix_Volume(m_channel, volume);
    m_sent_volume = volume;
  }
  if (fresh || pan_left != m_sent_pan_left || pan_right != m_sent_pan_right) {
    Mix_SetPanning(m_channel, static_cast<Uint8>(pan_left), static_cast<Uint8>(pan_right));
    m_sent_pan_left = pan_left;
    m_sent_pan_right = pan_right;
  }
}

/* EOF */
