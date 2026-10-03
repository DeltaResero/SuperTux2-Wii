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

#ifndef HEADER_SUPERTUX_AUDIO_SDL_STREAM_HPP
#define HEADER_SUPERTUX_AUDIO_SDL_STREAM_HPP

#include <array>
#include <atomic>
#include <cmath>
#include <cstdint>
#include <memory>
#include <vector>

#include <SDL_stdinc.h>

class SoundFile;

/** OpenAL Soft's default resampler, a 4 point Lagrange curve: how much the sample before a point mu of the way along,
    the two either side of it and the one after count */
inline std::array<float, 4> lagrange_weights(float mu)
{
  return { -mu * (mu - 1.0f) * (mu - 2.0f) / 6.0f,
           (mu + 1.0f) * (mu - 1.0f) * (mu - 2.0f) / 2.0f,
           -(mu + 1.0f) * mu * (mu - 2.0f) / 2.0f,
           (mu + 1.0f) * mu * (mu - 1.0f) / 6.0f };
}

/** OpenAL Soft's step through a sound at rate played at mixer_rate and pitch, 16.16 fixed point */
inline uint32_t resample_step(int rate, int mixer_rate, float pitch)
{
  return static_cast<uint32_t>(std::lround(static_cast<float>(rate) / static_cast<float>(mixer_rate) * pitch * 65536.0f));
}

/** A sound read from its file a little at a time, for SDL_mixer to play as it goes.
    It's read and resampled on the main thread, and taken by SDL_mixer's own thread. */
class SDLStream final
{
public:
  /** Reads the sound up to seconds ahead of where it's playing */
  SDLStream(std::unique_ptr<SoundFile> file, int rate, bool looping, size_t seconds);
  ~SDLStream();

  /** Reads ahead until the buffer is full, on the main thread */
  void fill();

  /** Takes up to frames of stereo samples, on SDL_mixer's thread, returning how many there were */
  size_t read(Sint16* out, size_t frames);

  /** Whether the file was stereo, as OpenAL plays a mono one quieter */
  bool stereo() const { return m_stereo; }

  /** Whether a sound that doesn't loop has been read to its end and all of it taken, on the main thread */
  bool finished() const;

  /** Writes the next stretch of the sound over a channel's silence, on SDL_mixer's thread */
  static void SDLCALL feed(int channel, void* stream, int len, void* sdl_stream);

private:
  /** Appends the file's next samples to m_source, starting it over at its end when looping */
  bool read_file();

private:
  std::unique_ptr<SoundFile> m_file;
  bool m_stereo;
  bool m_looping;
  bool m_ended;
  /** Whether everything up to the end has been resampled into the buffer */
  bool m_complete;
  /** Whether the file was just started over, so an empty read after it means it's empty */
  bool m_just_reset;

  /** How far each sample moves through the file's own samples, 16.16 fixed point, as in OpenAL Soft */
  uint32_t m_step;
  uint32_t m_fraction;

  /** The file's samples waiting to be resampled, stereo floats, and where in them the next one comes from */
  std::vector<float> m_source;
  size_t m_position;
  std::vector<char> m_bytes;

  /** Ready samples, written by fill() and taken by read() */
  std::vector<Sint16> m_ring;
  size_t m_frames;
  std::atomic<size_t> m_written;
  std::atomic<size_t> m_taken;

private:
  SDLStream(const SDLStream&) = delete;
  SDLStream& operator=(const SDLStream&) = delete;
};

#endif

/* EOF */
