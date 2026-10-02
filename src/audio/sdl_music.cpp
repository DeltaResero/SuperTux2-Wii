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

#include "audio/sdl_music.hpp"

#include <algorithm>
#include <cmath>

#include "audio/sound_file.hpp"
#include "supertux/globals.hpp"

namespace {

// OpenAL plays a mono sound at its listener this loud in each ear
const float CENTRE = 0.5956f;

// OpenAL Soft eases a change of level in over this many samples, which keeps a fade's steps from clicking
const size_t EASE = 64;

} // namespace

SDLMusic::SDLMusic(std::unique_ptr<SoundFile> file, int rate) :
  m_stream(std::move(file), rate, true),
  m_state(State::INITIAL),
  m_fade_state(NoFading),
  m_fade_start_time(),
  m_fade_time(),
  m_gain(1.0f),
  m_volume(1.0f),
  m_running(false),
  m_level(0.0f),
  m_heard(0.0f)
{
  send_level();
  m_heard = m_level;
}

void
SDLMusic::play()
{
  // Like OpenAL, where a stopped stream has let go of its buffers and has nothing left to play
  if (m_state == State::STOPPED)
    return;

  m_state = State::PLAYING;
  m_running = true;
}

void
SDLMusic::stop()
{
  m_state = State::STOPPED;
  m_running = false;
}

void
SDLMusic::pause()
{
  if (m_state != State::PLAYING)
    return;

  m_state = State::PAUSED;
  m_running = false;
}

void
SDLMusic::resume()
{
  if (!paused())
    return;

  play();
}

bool
SDLMusic::playing() const
{
  return m_state == State::PLAYING;
}

bool
SDLMusic::paused() const
{
  return m_state == State::PAUSED;
}

void
SDLMusic::set_volume(float volume)
{
  m_volume = volume;
  send_level();
}

void
SDLMusic::set_gain(float gain)
{
  m_gain = gain;
  send_level();
}

void
SDLMusic::send_level()
{
  // OpenAL never lets a source at its listener play louder than the file
  const float level = std::min(m_gain * m_volume, 1.0f);
  m_level = m_stream.stereo() ? level : CENTRE * level;
}

void
SDLMusic::set_fading(FadeState state, float fadetime)
{
  m_fade_state = state;
  m_fade_time = fadetime;
  m_fade_start_time = g_real_time;
}

void
SDLMusic::update()
{
  if (m_state == State::STOPPED)
    return;

  m_stream.fill();

  if (!playing() && !paused())
    return;

  if (m_fade_state == FadingOn || m_fade_state == FadingResume) {
    float time = g_real_time - m_fade_start_time;
    if (time >= m_fade_time) {
      set_gain(1.0);
      m_fade_state = NoFading;
    } else {
      set_gain(time / m_fade_time);
    }
  } else if (m_fade_state == FadingOff || m_fade_state == FadingPause) {
    float time = g_real_time - m_fade_start_time;
    if (time >= m_fade_time) {
      if (m_fade_state == FadingOff)
        stop();
      else
        pause();
      m_fade_state = NoFading;
    } else {
      set_gain( (m_fade_time - time) / m_fade_time);
    }
  }
}

void SDLCALL
SDLMusic::feed(void* music, Uint8* stream, int len)
{
  auto& self = *static_cast<SDLMusic*>(music);

  // SDL_mixer hands over silence, so a paused or stopped track just leaves it
  if (!self.m_running)
    return;

  auto out = reinterpret_cast<Sint16*>(stream);
  const size_t got = self.m_stream.read(out, static_cast<size_t>(len) / (2 * sizeof(Sint16)));
  const float level = self.m_level;
  const float from = self.m_heard;
  for (size_t i = 0; i < got; ++i) {
    const float gain = (i < EASE) ? from + (level - from) * static_cast<float>(i + 1) / static_cast<float>(EASE) : level;
    for (size_t ch = 0; ch < 2; ++ch) {
      const float value = static_cast<float>(out[i * 2 + ch]) * gain;
      out[i * 2 + ch] = static_cast<Sint16>(std::clamp(std::lround(value), -32768L, 32767L));
    }
  }
  if (got > 0)
    self.m_heard = (got >= EASE) ? level : from + (level - from) * static_cast<float>(got) / static_cast<float>(EASE);
}

/* EOF */
