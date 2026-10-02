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

#ifndef HEADER_SUPERTUX_AUDIO_SDL_MIXER_DEVICE_HPP
#define HEADER_SUPERTUX_AUDIO_SDL_MIXER_DEVICE_HPP

#include <map>
#include <string>
#include <vector>

#include <SDL_mixer.h>

#include "audio/audio_device.hpp"
#include "math/vector.hpp"

class SDLSoundSource;

/** SDL_mixer, with every sound read whole, as it can only play a sound it holds */
class SDLMixerDevice final : public AudioDevice
{
public:
  SDLMixerDevice();
  ~SDLMixerDevice() override;

  bool is_open() const override { return m_open; }

  std::unique_ptr<SoundSource> create_source(const std::string& filename, bool full) override;
  void preload(const std::string& filename) override;

  // SDL_mixer doesn't play the music yet
  void play_music(const std::string& , float , float ) override {}
  void keep_music_playing() override {}
  void stop_music(float ) override {}
  void pause_music(float ) override {}
  void resume_music(float ) override {}
  void set_music_volume(float ) override {}
  bool has_music() const override { return false; }

  void set_listener_position(const Vector& position) override;
  void set_listener_orientation(const Vector& , const Vector& ) override {}

  void update() override {}

  const Vector& get_listener_position() const { return m_listener; }

  /** A free channel marked with a new play, or -1 when every channel is busy */
  int claim_channel(unsigned& play);

  /** Whether channel is still playing the play it was claimed for */
  bool carries(int channel, unsigned play) const;

  void add_source(SDLSoundSource& source);
  void remove_source(SDLSoundSource& source);

private:
  struct Chunk
  {
    Mix_Chunk* chunk;
    bool stereo;
  };

  const Chunk& get_chunk(const std::string& filename);

private:
  bool m_open;
  std::map<std::string, Chunk> m_chunks;
  Vector m_listener;

  /** The play each channel was last claimed for, so a sound never takes another's channel for its own */
  std::vector<unsigned> m_channel_plays;
  unsigned m_plays;

  std::vector<SDLSoundSource*> m_sources;

private:
  SDLMixerDevice(const SDLMixerDevice&) = delete;
  SDLMixerDevice& operator=(const SDLMixerDevice&) = delete;
};

#endif

/* EOF */
