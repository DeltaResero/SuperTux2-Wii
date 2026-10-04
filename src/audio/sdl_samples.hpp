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

#ifndef HEADER_SUPERTUX_AUDIO_SDL_SAMPLES_HPP
#define HEADER_SUPERTUX_AUDIO_SDL_SAMPLES_HPP

#include <atomic>
#include <cstddef>
#include <memory>
#include <vector>

#include <SDL_stdinc.h>

class SoundFile;

/** A sound at its own rate, as OpenAL holds it, read from its file all at once or a little ahead of where it's played.
    It's read on the main thread and played from SDL_mixer's own thread. */
class SDLSamples final
{
public:
  explicit SDLSamples(std::unique_ptr<SoundFile> file);
  ~SDLSamples();

  /** Reads on as far as frame, or to the end, letting go of the file once it's all read */
  void read_to(size_t frame);

  /** How many frames have been read, safe to play from SDL_mixer's thread */
  size_t ready() const { return m_ready.load(std::memory_order_acquire); }

  const Sint16* data() const { return m_data.data(); }
  size_t frames() const { return m_frames; }
  int channels() const { return m_channels; }
  int rate() const { return m_rate; }

private:
  /** The file, until it's all been read */
  std::unique_ptr<SoundFile> m_file;
  int m_channels;
  int m_rate;
  bool m_eight_bit;
  size_t m_frames;
  /** The whole sound, silent where it hasn't been read yet */
  std::vector<Sint16> m_data;
  std::atomic<size_t> m_ready;

private:
  SDLSamples(const SDLSamples&) = delete;
  SDLSamples& operator=(const SDLSamples&) = delete;
};

#endif

/* EOF */
