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

class SDLMusic;
class SDLSoundSource;
class SoundFile;

/** SDL_mixer, holding the short sounds whole and feeding it the long ones and the music as they play */
class SDLMixerDevice final : public AudioDevice
{
public:
  /** How far ahead a long sound is read, in seconds, a few times the wait between updates */
  static constexpr size_t READ_AHEAD = 1;

public:
  SDLMixerDevice();
  ~SDLMixerDevice() override;

  bool is_open() const override { return m_open; }

  std::unique_ptr<SoundSource> create_source(const std::string& filename, bool full) override;
  void preload(const std::string& filename) override;

  void play_music(const std::string& filename, float fadetime, float volume) override;
  void keep_music_playing() override;
  void stop_music(float fadetime) override;
  void pause_music(float fadetime) override;
  void resume_music(float fadetime) override;
  void set_music_volume(float volume) override;
  bool has_music() const override { return m_music_source != nullptr; }

  void set_listener_position(const Vector& position) override;
  void set_listener_orientation(const Vector& , const Vector& ) override {}

  void update() override;

  const Vector& get_listener_position() const { return m_listener; }

  /** A free channel marked with a new play, or -1 when every channel is busy */
  int claim_channel(unsigned& play);

  /** Whether channel is still playing the play it was claimed for */
  bool carries(int channel, unsigned play) const;

  /** A sound read whole at its own rate, for one played faster or slower */
  struct Samples
  {
    std::vector<Sint16> data{};
    int channels = 1;
    int rate = 0;
  };
  const Samples& get_samples(const std::string& filename);

  /** The silence a channel plays while an SDLVoice writes over it */
  Mix_Chunk* get_silence() const { return m_silence; }

  int get_rate() const { return m_rate; }

  void add_source(SDLSoundSource& source);
  void remove_source(SDLSoundSource& source);

private:
  struct Chunk
  {
    Mix_Chunk* chunk;
    bool stereo;
  };

  /** Reads a short sound whole and keeps it */
  const Chunk& hold(const std::string& filename, std::unique_ptr<SoundFile> file);

  /** Swaps in a new track, or none, while SDL_mixer isn't reading the old one */
  void set_music(std::unique_ptr<SDLMusic> music);

private:
  bool m_open;
  int m_rate;
  std::map<std::string, Chunk> m_chunks;
  Vector m_listener;

  /** The play each channel was last claimed for, so a sound never takes another's channel for its own */
  std::vector<unsigned> m_channel_plays;
  unsigned m_plays;

  std::vector<SDLSoundSource*> m_sources;

  std::map<std::string, std::unique_ptr<Samples>> m_samples;
  Mix_Chunk* m_silence;

  std::unique_ptr<SDLMusic> m_music_source;

private:
  SDLMixerDevice(const SDLMixerDevice&) = delete;
  SDLMixerDevice& operator=(const SDLMixerDevice&) = delete;
};

#endif

/* EOF */
