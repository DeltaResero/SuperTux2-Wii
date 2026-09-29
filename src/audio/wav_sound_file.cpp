//  SuperTux
//  Copyright (C) 2006 Matthias Braun <matze@braunis.de>
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

#include "audio/wav_sound_file.hpp"

#include <config.h>

#include <string.h>
#include <stdint.h>
#include <assert.h>
#include <algorithm>

#include "audio/sound_error.hpp"
#include "util/log.hpp"

// WAVE integers are little endian on every host, so build them a byte at a time.
static inline uint32_t read32LE(std::istream& file)
{
  unsigned char b[4];
  if (!file.read(reinterpret_cast<char*>(b), sizeof(b)))
    throw SoundError("file too short");

  return static_cast<uint32_t>(b[0])
    | (static_cast<uint32_t>(b[1]) << 8)
    | (static_cast<uint32_t>(b[2]) << 16)
    | (static_cast<uint32_t>(b[3]) << 24);
}

static inline uint16_t read16LE(std::istream& file)
{
  unsigned char b[2];
  if (!file.read(reinterpret_cast<char*>(b), sizeof(b)))
    throw SoundError("file too short");

  return static_cast<uint16_t>(b[0] | (b[1] << 8));
}

WavSoundFile::WavSoundFile(std::unique_ptr<std::istream> file_) :
  m_file(std::move(file_)),
  m_datastart()
{
  assert(m_file);
  char magic[4];
  if (!m_file->read(magic, sizeof(magic)))
    throw SoundError("Couldn't read file magic (not a wave file)");
  if (strncmp(magic, "RIFF", 4) != 0) {
    log_debug << "MAGIC: " << magic << std::endl;
    throw SoundError("file is not a RIFF wav file");
  }

  uint32_t wavelen = read32LE(*m_file);
  (void) wavelen;

  if (!m_file->read(magic, sizeof(magic)))
    throw SoundError("Couldn't read chunk header (not a wav file?)");
  if (strncmp(magic, "WAVE", 4) != 0)
    throw SoundError("file is not a valid RIFF/WAVE file");

  char chunkmagic[4];
  uint32_t chunklen;

  // search audio data format chunk
  do {
    if (!m_file->read(chunkmagic, sizeof(chunkmagic)))
      throw SoundError("EOF while searching format chunk");
    chunklen = read32LE(*m_file);

    if (strncmp(chunkmagic, "fmt ", 4) == 0)
      break;

    if (strncmp(chunkmagic, "fact", 4) == 0
       || strncmp(chunkmagic, "LIST", 4) == 0) {
      // skip chunk
      if (!m_file->seekg(static_cast<std::streamoff>(chunklen), std::ios::cur))
        throw SoundError("EOF while searching fmt chunk");
    } else {
      throw SoundError("complex WAVE files not supported");
    }
  } while(true);

  if (chunklen < 16)
    throw SoundError("Format chunk too short");

  // parse format
  uint16_t encoding = read16LE(*m_file);
  if (encoding != 1)
    throw SoundError("only PCM encoding supported");
  m_channels = read16LE(*m_file);
  m_rate = read32LE(*m_file);
  uint32_t byterate = read32LE(*m_file);
  (void) byterate;
  uint16_t blockalign = read16LE(*m_file);
  (void) blockalign;
  m_bits_per_sample = read16LE(*m_file);

  if (chunklen > 16) {
    if (!m_file->seekg(static_cast<std::streamoff>(chunklen - 16), std::ios::cur))
      throw SoundError("EOF while reading rest of format chunk");
  }

  // set file offset to DATA chunk data
  do {
    if (!m_file->read(chunkmagic, sizeof(chunkmagic)))
      throw SoundError("EOF while searching data chunk");
    chunklen = read32LE(*m_file);

    if (strncmp(chunkmagic, "data", 4) == 0)
      break;

    // skip chunk
    if (!m_file->seekg(static_cast<std::streamoff>(chunklen), std::ios::cur))
      throw SoundError("EOF while searching fmt chunk");
  } while(true);

  m_datastart = m_file->tellg();
  m_size = static_cast<size_t>(chunklen);
}

WavSoundFile::~WavSoundFile()
{
}

void
WavSoundFile::reset()
{
  m_file->clear();
  if (!m_file->seekg(m_datastart))
    throw SoundError("Couldn't seek to data start");
}

size_t
WavSoundFile::read(void* buffer, size_t buffer_size)
{
  const std::streampos end = m_datastart + static_cast<std::streamoff>(m_size);
  const std::streampos cur = m_file->tellg();
  if (cur >= end)
    return 0;

  size_t readsize = std::min(static_cast<size_t> (end - cur), buffer_size);
  if (!m_file->read(static_cast<char*>(buffer), static_cast<std::streamsize>(readsize)))
    throw SoundError("read error while reading samples");

#ifdef WORDS_BIGENDIAN
  if (m_bits_per_sample != 16)
    return readsize;
  char *tmp = (char*)buffer;

  for (size_t i = 0; i < readsize / 2; i++)
  {
    char c     = tmp[2*i];
    tmp[2*i]   = tmp[2*i+1];
    tmp[2*i+1] = c;
  }

  *(char *)buffer = *tmp;
#endif

  return readsize;
}

/* EOF */
