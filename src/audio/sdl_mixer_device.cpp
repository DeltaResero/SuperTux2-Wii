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
#include <cstring>
#include <memory>
#include <stdexcept>

#include "audio/sdl_music.hpp"
#include "audio/sdl_sound_source.hpp"
#include "audio/sdl_stream.hpp"
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
  m_rate(RATE),
  m_chunks(),
  m_listener(0.0f, 0.0f),
  m_channel_plays(),
  m_plays(0),
  m_sources(),
  m_samples(),
  m_silence(nullptr),
  m_music_source()
{
  if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0) {
    log_warning << "Couldn't initialize audio device: " << SDL_GetError() << std::endl;
    return;
  }
  // The music is written straight into the mix, so the samples have to stay 16 bit stereo
  if (Mix_OpenAudioDevice(RATE, AUDIO_S16SYS, 2, FRAGMENT, nullptr, SDL_AUDIO_ALLOW_FREQUENCY_CHANGE) < 0) {
    log_warning << "Couldn't initialize audio device: " << Mix_GetError() << std::endl;
    SDL_QuitSubSystem(SDL_INIT_AUDIO);
    return;
  }
  Mix_QuerySpec(&m_rate, nullptr, nullptr);
  Mix_AllocateChannels(CHANNELS);
  m_channel_plays.assign(CHANNELS, 0);

  const Uint32 silence = FRAGMENT * 2 * sizeof(Sint16);
  auto data = static_cast<Uint8*>(SDL_calloc(1, silence));
  m_silence = (data != nullptr) ? Mix_QuickLoad_RAW(data, silence) : nullptr;
  if (m_silence != nullptr)
    m_silence->allocated = 1;
  else
    SDL_free(data);

  m_open = true;
}

SDLMixerDevice::~SDLMixerDevice()
{
  if (!m_open)
    return;

  // No channel can still be playing a chunk that's freed, nor the mixer reading the music
  set_music(nullptr);
  Mix_HaltChannel(-1);
  for (const auto& chunk : m_chunks) {
    Mix_FreeChunk(chunk.second.chunk);
  }
  if (m_silence != nullptr)
    Mix_FreeChunk(m_silence);
  Mix_CloseAudio();
  SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

const SDLMixerDevice::Chunk&
SDLMixerDevice::hold(const std::string& filename, std::unique_ptr<SoundFile> file)
{
  // A chunk holds the mixer's own format, so the sound is read through the music's resampler, as OpenAL plays it
  const bool stereo = file->m_channels == 2;
  SDLStream stream(std::move(file), m_rate, false, READ_AHEAD);
  std::vector<Sint16> samples;
  std::vector<Sint16> part(4096 * 2);
  while (const size_t got = stream.read(part.data(), 4096)) {
    samples.insert(samples.end(), part.begin(), part.begin() + static_cast<std::ptrdiff_t>(got * 2));
    stream.fill();
  }

  const size_t size = samples.size() * sizeof(Sint16);
  auto data = static_cast<Uint8*>(SDL_malloc(size));
  if (data == nullptr)
    throw std::runtime_error("Couldn't hold sound '" + filename + "'");
  std::memcpy(data, samples.data(), size);

  Mix_Chunk* chunk = Mix_QuickLoad_RAW(data, static_cast<Uint32>(size));
  if (chunk == nullptr) {
    SDL_free(data);
    throw std::runtime_error("Couldn't hold sound: " + std::string(Mix_GetError()));
  }
  // So that Mix_FreeChunk frees the samples with it
  chunk->allocated = 1;

  return m_chunks.emplace(filename, Chunk{chunk, stereo}).first->second;
}

const SDLMixerDevice::Samples&
SDLMixerDevice::get_samples(const std::string& filename)
{
  auto it = m_samples.find(filename);
  if (it != m_samples.end())
    return *it->second;

  std::unique_ptr<SoundFile> file(load_sound_file(filename));
  auto samples = std::make_unique<Samples>();
  samples->channels = file->m_channels;
  samples->rate = file->m_rate;
  std::vector<char> bytes(file->m_size);
  const size_t got = file->read(bytes.data(), bytes.size());
  if (file->m_bits_per_sample == 8) {
    samples->data.resize(got);
    for (size_t i = 0; i < got; ++i)
      samples->data[i] = static_cast<Sint16>((static_cast<unsigned char>(bytes[i]) - 128) * 256);
  } else {
    samples->data.resize(got / sizeof(Sint16));
    std::memcpy(samples->data.data(), bytes.data(), samples->data.size() * sizeof(Sint16));
  }
  return *m_samples.emplace(filename, std::move(samples)).first->second;
}

std::unique_ptr<SoundSource>
SDLMixerDevice::create_source(const std::string& filename, bool full)
{
  auto it = m_chunks.find(filename);
  if (it != m_chunks.end())
    return std::make_unique<SDLSoundSource>(*this, filename, it->second.chunk, it->second.stereo, full);

  // Like OpenAL, a long sound is read from its file as it plays instead of being held
  std::unique_ptr<SoundFile> file(load_sound_file(filename));
  if (file->m_size >= STREAM_FROM)
    return std::make_unique<SDLSoundSource>(*this, filename, std::move(file), full);

  const Chunk& chunk = hold(filename, std::move(file));
  return std::make_unique<SDLSoundSource>(*this, filename, chunk.chunk, chunk.stereo, full);
}

void
SDLMixerDevice::preload(const std::string& filename)
{
  if (m_chunks.find(filename) != m_chunks.end())
    return;

  try {
    std::unique_ptr<SoundFile> file(load_sound_file(filename));
    if (file->m_size < STREAM_FROM)
      hold(filename, std::move(file));
  } catch(std::exception& e) {
    log_warning << "Error while preloading sound file: " << e.what() << std::endl;
  }
}

void
SDLMixerDevice::set_music(std::unique_ptr<SDLMusic> music)
{
  Mix_HookMusic(nullptr, nullptr);
  m_music_source = std::move(music);
  if (m_music_source)
    Mix_HookMusic(SDLMusic::feed, m_music_source.get());
}

void
SDLMixerDevice::play_music(const std::string& filename, float fadetime, float volume)
{
  auto newmusic = std::make_unique<SDLMusic>(load_sound_file(filename), m_rate);
  newmusic->set_volume(volume);
  if (fadetime > 0)
    newmusic->set_fading(SDLMusic::FadingOn, fadetime);
  newmusic->play();

  set_music(std::move(newmusic));
}

void
SDLMixerDevice::keep_music_playing()
{
  if (m_music_source == nullptr)
    return;

  if (m_music_source->paused())
  {
    m_music_source->resume();
  }
  else if (!m_music_source->playing())
  {
    m_music_source->play();
  }
}

void
SDLMixerDevice::stop_music(float fadetime)
{
  if (fadetime > 0) {
    if (m_music_source
        && m_music_source->get_fade_state() != SDLMusic::FadingOff)
      m_music_source->set_fading(SDLMusic::FadingOff, fadetime);
  } else {
    set_music(nullptr);
  }
}

void
SDLMixerDevice::pause_music(float fadetime)
{
  if (m_music_source == nullptr)
    return;

  if (fadetime > 0) {
    if (m_music_source
        && m_music_source->get_fade_state() != SDLMusic::FadingPause)
      m_music_source->set_fading(SDLMusic::FadingPause, fadetime);
  } else {
    m_music_source->pause();
  }
}

void
SDLMixerDevice::resume_music(float fadetime)
{
  if (m_music_source == nullptr)
    return;

  if (fadetime > 0) {
    if (m_music_source
        && m_music_source->get_fade_state() != SDLMusic::FadingResume) {
      m_music_source->set_fading(SDLMusic::FadingResume, fadetime);
      m_music_source->resume();
    }
  } else {
    m_music_source->resume();
  }
}

void
SDLMixerDevice::set_music_volume(float volume)
{
  if (m_music_source != nullptr) m_music_source->set_volume(volume);
}

void
SDLMixerDevice::update()
{
  if (m_music_source) {
    m_music_source->update();
  }

  for (auto* source : m_sources) {
    source->keep_up();
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
