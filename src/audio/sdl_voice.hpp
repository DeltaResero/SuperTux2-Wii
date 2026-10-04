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

#ifndef HEADER_SUPERTUX_AUDIO_SDL_VOICE_HPP
#define HEADER_SUPERTUX_AUDIO_SDL_VOICE_HPP

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <memory>

#include <SDL_stdinc.h>

class SDLSamples;

/** A sound at its own rate, played by writing it over the silence an SDL_mixer channel plays.
    It's resampled as it goes, the way OpenAL plays a sound at another rate or pitch, which SDL_mixer can't do itself. */
class SDLVoice final
{
public:
  SDLVoice(std::shared_ptr<SDLSamples> sound, uint32_t step, bool looping);

  void set_step(uint32_t step) { m_step = step; }

  const SDLSamples& get_sound() const { return *m_sound; }

  /** Reads the sound on as far as output_frames of the mixer's past where the voice has got to, on the main thread */
  void read_ahead(size_t output_frames);

  /** Whether a sound that doesn't loop has played to its end */
  bool finished() const { return m_finished; }

  /** Writes the next stretch of the sound over the channel's silence, on SDL_mixer's thread */
  static void SDLCALL feed(int channel, void* stream, int len, void* voice);

private:
  /** One of the sound's samples, silence before the start and after the end unless it loops */
  float sample(ptrdiff_t frame, int channel) const;

private:
  /** Kept for as long as the voice plays it */
  std::shared_ptr<SDLSamples> m_sound;
  const Sint16* m_samples;
  size_t m_frames;
  int m_channels;
  std::atomic<uint32_t> m_step;
  bool m_looping;

  /** Where the voice is in the sound, and whether it's gone round once, on SDL_mixer's thread only */
  size_t m_position;
  uint32_t m_fraction;
  bool m_looped;

  /** Where the voice had got to after its last stretch, for the main thread to read ahead of */
  std::atomic<size_t> m_reached;
  std::atomic<bool> m_finished;

private:
  SDLVoice(const SDLVoice&) = delete;
  SDLVoice& operator=(const SDLVoice&) = delete;
};

#endif

/* EOF */
