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

#include "audio/sdl_samples.hpp"

#include <algorithm>
#include <stdexcept>

#include "audio/sound_file.hpp"

namespace {

// How much of the file is read at a time
const size_t BLOCK = 4096;

} // namespace

SDLSamples::SDLSamples(std::unique_ptr<SoundFile> file) :
  m_file(std::move(file)),
  m_channels(m_file->m_channels),
  m_rate(m_file->m_rate),
  m_eight_bit(m_file->m_bits_per_sample == 8),
  m_frames(),
  m_data(),
  m_ready(0)
{
  if (m_channels != 1 && m_channels != 2)
    throw std::runtime_error("Only 1 and 2 channel samples supported");
  if (m_file->m_bits_per_sample != 8 && m_file->m_bits_per_sample != 16)
    throw std::runtime_error("Only 16 and 8 bit samples supported");

  m_frames = m_file->m_size / static_cast<size_t>(m_channels * (m_eight_bit ? 1 : 2));
  m_data.resize(m_frames * static_cast<size_t>(m_channels));
}

SDLSamples::~SDLSamples()
{
}

void
SDLSamples::read_to(size_t frame)
{
  if (!m_file)
    return;

  const auto channels = static_cast<size_t>(m_channels);
  size_t ready = m_ready.load(std::memory_order_relaxed);
  std::vector<unsigned char> bytes;
  while (ready < std::min(frame, m_frames)) {
    const size_t count = std::min(BLOCK, m_frames - ready);
    Sint16* at = &m_data[ready * channels];
    size_t got;
    if (m_eight_bit) {
      bytes.resize(count * channels);
      got = m_file->read(bytes.data(), bytes.size()) / channels;
      for (size_t i = 0; i < got * channels; ++i)
        at[i] = static_cast<Sint16>((bytes[i] - 128) * 256);
    } else {
      got = m_file->read(at, count * channels * sizeof(Sint16)) / (channels * sizeof(Sint16));
    }

    // A file that comes up short of what it said it held ends in silence
    if (got == 0) {
      ready = m_frames;
      break;
    }
    ready += got;
  }

  if (ready >= m_frames)
    m_file.reset();
  m_ready.store(ready, std::memory_order_release);
}

/* EOF */
