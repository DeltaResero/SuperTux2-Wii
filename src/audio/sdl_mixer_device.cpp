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

#include "audio/sdl_mixer_device.hpp"

#include <SDL.h>
#include <algorithm>
#include <memory>
#include <stdexcept>

#include "audio/sdl_sound_source.hpp"
#include "audio/sound_file.hpp"
#include "util/log.hpp"

namespace {

// The rate of the game's music and nearly all of its sounds
const int RATE = 44100;

// About 23 ms of sound a refill, close to OpenAL Soft's own wait
const int FRAGMENT = 1024;

// How many sounds can play at once, well past what a busy level asks for
const int CHANNELS = 64;

} // namespace

SDLMixerDevice::SDLMixerDevice() :
  m_open(false),
  m_chunks(),
  m_listener(0.0f, 0.0f),
  m_channel_plays(),
  m_plays(0),
  m_sources()
{
  if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0) {
    log_warning << "Couldn't initialize audio device: " << SDL_GetError() << std::endl;
    return;
  }
  if (Mix_OpenAudio(RATE, MIX_DEFAULT_FORMAT, 2, FRAGMENT) < 0) {
    log_warning << "Couldn't initialize audio device: " << Mix_GetError() << std::endl;
    SDL_QuitSubSystem(SDL_INIT_AUDIO);
    return;
  }
  Mix_AllocateChannels(CHANNELS);
  m_channel_plays.assign(CHANNELS, 0);
  m_open = true;
}

SDLMixerDevice::~SDLMixerDevice()
{
  if (!m_open)
    return;

  // No channel can still be playing a chunk that's freed
  Mix_HaltChannel(-1);
  for (const auto& chunk : m_chunks) {
    Mix_FreeChunk(chunk.second.chunk);
  }
  Mix_CloseAudio();
  SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

const SDLMixerDevice::Chunk&
SDLMixerDevice::get_chunk(const std::string& filename)
{
  auto it = m_chunks.find(filename);
  if (it != m_chunks.end())
    return it->second;

  std::unique_ptr<SoundFile> file(load_sound_file(filename));
  if (file->m_channels != 1 && file->m_channels != 2)
    throw std::runtime_error("Only 1 and 2 channel samples supported");
  if (file->m_bits_per_sample != 8 && file->m_bits_per_sample != 16)
    throw std::runtime_error("Only 16 and 8 bit samples supported");

  std::vector<char> samples(file->m_size);
  const size_t got = file->read(samples.data(), samples.size());

  // A chunk has to be in the mixer's own format, so the sound is converted once, here
  int rate = 0;
  Uint16 format = 0;
  int channels = 0;
  Mix_QuerySpec(&rate, &format, &channels);
  SDL_AudioStream* stream = SDL_NewAudioStream(file->m_bits_per_sample == 8 ? AUDIO_U8 : AUDIO_S16SYS,
                                               static_cast<Uint8>(file->m_channels), file->m_rate,
                                               format, static_cast<Uint8>(channels), rate);
  if (stream == nullptr)
    throw std::runtime_error("Couldn't convert sound: " + std::string(SDL_GetError()));
  SDL_AudioStreamPut(stream, samples.data(), static_cast<int>(got));
  SDL_AudioStreamFlush(stream);
  const int size = SDL_AudioStreamAvailable(stream);
  auto data = static_cast<Uint8*>(SDL_malloc(static_cast<size_t>(size)));
  if (data != nullptr)
    SDL_AudioStreamGet(stream, data, size);
  SDL_FreeAudioStream(stream);
  if (data == nullptr)
    throw std::runtime_error("Couldn't hold sound '" + filename + "'");

  Mix_Chunk* chunk = Mix_QuickLoad_RAW(data, static_cast<Uint32>(size));
  if (chunk == nullptr) {
    SDL_free(data);
    throw std::runtime_error("Couldn't hold sound: " + std::string(Mix_GetError()));
  }
  // So that Mix_FreeChunk frees the samples with it
  chunk->allocated = 1;

  return m_chunks.emplace(filename, Chunk{chunk, file->m_channels == 2}).first->second;
}

std::unique_ptr<SoundSource>
SDLMixerDevice::create_source(const std::string& filename, bool full)
{
  const Chunk& chunk = get_chunk(filename);
  return std::make_unique<SDLSoundSource>(*this, chunk.chunk, chunk.stereo, full);
}

void
SDLMixerDevice::preload(const std::string& filename)
{
  try {
    get_chunk(filename);
  } catch(std::exception& e) {
    log_warning << "Error while preloading sound file: " << e.what() << std::endl;
  }
}

void
SDLMixerDevice::set_listener_position(const Vector& position)
{
  m_listener = position;
  for (auto* source : m_sources) {
    source->listener_moved();
  }
}

int
SDLMixerDevice::claim_channel(unsigned& play)
{
  const int channel = Mix_GroupAvailable(-1);
  if (channel < 0)
    return -1;

  play = ++m_plays;
  m_channel_plays[static_cast<size_t>(channel)] = play;
  return channel;
}

bool
SDLMixerDevice::carries(int channel, unsigned play) const
{
  return channel >= 0 && m_channel_plays[static_cast<size_t>(channel)] == play && Mix_Playing(channel) != 0;
}

void
SDLMixerDevice::add_source(SDLSoundSource& source)
{
  m_sources.push_back(&source);
}

void
SDLMixerDevice::remove_source(SDLSoundSource& source)
{
  m_sources.erase(std::remove(m_sources.begin(), m_sources.end(), &source), m_sources.end());
}

/* EOF */
