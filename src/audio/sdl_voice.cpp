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

#include "audio/sdl_voice.hpp"

#include <algorithm>
#include <cmath>
#include <cstring>

#include "audio/sdl_stream.hpp"

namespace {

// A step of exactly one sample, in 16.16 fixed point
const uint32_t SAME_RATE = 1 << 16;

} // namespace

SDLVoice::SDLVoice(const Sint16* samples, size_t frames, int channels, uint32_t step, bool looping) :
  m_samples(samples),
  m_frames(frames),
  m_channels(channels),
  m_step(step),
  m_looping(looping),
  m_position(0),
  m_fraction(0),
  m_looped(false),
  m_finished(false)
{
}

float
SDLVoice::sample(ptrdiff_t frame, int channel) const
{
  const auto frames = static_cast<ptrdiff_t>(m_frames);
  if (frame >= frames || frame < 0) {
    // Like OpenAL, which starts from silence and only reaches back round the end once it's been there
    if (!m_looping || (frame < 0 && !m_looped))
      return 0.0f;
    frame = ((frame % frames) + frames) % frames;
  }
  const size_t at = static_cast<size_t>(frame) * static_cast<size_t>(m_channels) + ((m_channels == 2) ? static_cast<size_t>(channel) : 0);
  return static_cast<float>(m_samples[at]) / 32768.0f;
}

void SDLCALL
SDLVoice::feed(int , void* stream, int len, void* voice)
{
  auto& self = *static_cast<SDLVoice*>(voice);
  auto out = static_cast<Sint16*>(stream);
  const size_t count = static_cast<size_t>(len) / (2 * sizeof(Sint16));
  const uint32_t step = self.m_step;

  for (size_t i = 0; i < count; ++i) {
    if (self.m_position >= self.m_frames) {
      if (!self.m_looping || self.m_frames == 0) {
        std::memset(out + i * 2, 0, (count - i) * 2 * sizeof(Sint16));
        self.m_finished = true;
        return;
      }
      self.m_position -= self.m_frames;
      self.m_looped = true;
    }

    // At the mixer's own rate the curve lands right on each sample, so it's copied as it is
    if (step == SAME_RATE && self.m_fraction == 0) {
      const size_t at = self.m_position * static_cast<size_t>(self.m_channels);
      out[i * 2] = self.m_samples[at];
      out[i * 2 + 1] = self.m_samples[at + static_cast<size_t>(self.m_channels) - 1];
      ++self.m_position;
      continue;
    }

    const auto w = lagrange_weights(static_cast<float>(self.m_fraction) / 65536.0f);
    const auto at = static_cast<ptrdiff_t>(self.m_position);
    for (int ch = 0; ch < self.m_channels; ++ch) {
      const float value = w[0] * self.sample(at - 1, ch) + w[1] * self.sample(at, ch)
                          + w[2] * self.sample(at + 1, ch) + w[3] * self.sample(at + 2, ch);
      out[i * 2 + static_cast<size_t>(ch)] = static_cast<Sint16>(std::clamp(std::lround(value * 32768.0f), -32768L, 32767L));
    }
    // A mono sound is worked out once for both ears
    if (self.m_channels == 1)
      out[i * 2 + 1] = out[i * 2];

    self.m_fraction += step;
    self.m_position += self.m_fraction >> 16;
    self.m_fraction &= 0xFFFF;
  }
}

/* EOF */
