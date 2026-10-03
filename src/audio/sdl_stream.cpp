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

#include "audio/sdl_stream.hpp"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <stdexcept>

#include "audio/sound_file.hpp"

namespace {

// How much of the file is read at a time
const size_t BLOCK = 4096;

} // namespace

SDLStream::SDLStream(std::unique_ptr<SoundFile> file, int rate, bool looping, size_t seconds) :
  m_file(std::move(file)),
  m_stereo(m_file->m_channels == 2),
  m_looping(looping),
  m_ended(false),
  m_complete(false),
  m_just_reset(false),
  m_step(resample_step(m_file->m_rate, rate, 1.0f)),
  m_fraction(0),
  m_source(2, 0.0f),
  m_position(1),
  m_bytes(),
  m_ring(),
  m_frames(static_cast<size_t>(rate) * seconds),
  m_written(0),
  m_taken(0)
{
  if (m_file->m_channels != 1 && m_file->m_channels != 2)
    throw std::runtime_error("Only 1 and 2 channel samples supported");
  if (m_file->m_bits_per_sample != 8 && m_file->m_bits_per_sample != 16)
    throw std::runtime_error("Only 16 and 8 bit samples supported");

  m_ring.resize(m_frames * 2);
  fill();
}

SDLStream::~SDLStream()
{
}

bool
SDLStream::read_file()
{
  if (m_ended)
    return false;

  const size_t channels = m_stereo ? 2 : 1;
  const size_t width = static_cast<size_t>(m_file->m_bits_per_sample / 8);
  m_bytes.resize(BLOCK * channels * width);
  const size_t got = m_file->read(m_bytes.data(), m_bytes.size());

  if (got < m_bytes.size()) {
    // An empty stretch straight after starting over means there's nothing there to loop
    if (m_looping && (got > 0 || !m_just_reset)) {
      m_file->reset();
      m_just_reset = true;
    } else {
      m_ended = true;
    }
  } else {
    m_just_reset = false;
  }

  const size_t frames = got / (channels * width);
  const size_t start = m_source.size();
  m_source.resize(start + frames * 2);
  for (size_t i = 0; i < frames; ++i) {
    for (size_t ch = 0; ch < 2; ++ch) {
      const size_t at = i * channels + (m_stereo ? ch : 0);
      float sample;
      if (width == 2) {
        Sint16 value;
        std::memcpy(&value, &m_bytes[at * 2], 2);
        sample = static_cast<float>(value) / 32768.0f;
      } else {
        sample = (static_cast<float>(static_cast<unsigned char>(m_bytes[at])) - 128.0f) / 128.0f;
      }
      m_source[start + i * 2 + ch] = sample;
    }
  }

  // Silence after the last sample lets the resampler finish it
  if (m_ended)
    m_source.resize(m_source.size() + 4, 0.0f);
  return frames > 0 || m_ended || m_just_reset;
}

void
SDLStream::fill()
{
  size_t at = m_written.load(std::memory_order_relaxed);
  size_t room = m_frames - (at - m_taken.load(std::memory_order_acquire));

  while (room > 0) {
    // The curve needs the sample before and the two after where it is
    if (m_source.size() / 2 < m_position + 3) {
      m_source.erase(m_source.begin(), m_source.begin() + static_cast<std::ptrdiff_t>((m_position - 1) * 2));
      m_position = 1;
      if (!read_file()) {
        m_complete = true;
        break;
      }
      continue;
    }

    // Resampled as OpenAL Soft does, so a sound at another rate comes out as it does there
    const auto w = lagrange_weights(static_cast<float>(m_fraction) / 65536.0f);
    const float* in = &m_source[(m_position - 1) * 2];
    Sint16* out = &m_ring[(at % m_frames) * 2];
    for (size_t ch = 0; ch < 2; ++ch) {
      const float value = w[0] * in[ch] + w[1] * in[2 + ch] + w[2] * in[4 + ch] + w[3] * in[6 + ch];
      out[ch] = static_cast<Sint16>(std::clamp(std::lround(value * 32768.0f), -32768L, 32767L));
    }
    ++at;
    --room;

    m_fraction += m_step;
    m_position += m_fraction >> 16;
    m_fraction &= 0xFFFF;
  }

  m_written.store(at, std::memory_order_release);
}

size_t
SDLStream::read(Sint16* out, size_t frames)
{
  const size_t taken = m_taken.load(std::memory_order_relaxed);
  const size_t count = std::min(frames, m_written.load(std::memory_order_acquire) - taken);

  // The ring wraps at most once in a read
  const size_t from = taken % m_frames;
  const size_t first = std::min(count, m_frames - from);
  std::memcpy(out, &m_ring[from * 2], first * 2 * sizeof(Sint16));
  std::memcpy(out + first * 2, m_ring.data(), (count - first) * 2 * sizeof(Sint16));

  m_taken.store(taken + count, std::memory_order_release);
  return count;
}

bool
SDLStream::finished() const
{
  return m_complete && m_taken.load(std::memory_order_acquire) == m_written.load(std::memory_order_relaxed);
}

void SDLCALL
SDLStream::feed(int , void* stream, int len, void* sdl_stream)
{
  auto out = static_cast<Sint16*>(stream);
  const size_t frames = static_cast<size_t>(len) / (2 * sizeof(Sint16));
  const size_t got = static_cast<SDLStream*>(sdl_stream)->read(out, frames);

  // Silence past the end, or if the main thread fell behind
  std::memset(out + got * 2, 0, (frames - got) * 2 * sizeof(Sint16));
}

/* EOF */
