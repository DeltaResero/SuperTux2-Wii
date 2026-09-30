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

#include "audio/flac_sound_file.hpp"

#include <algorithm>
#include <cstring>

#include "audio/sound_error.hpp"
#include "util/log.hpp"

FlacSoundFile::FlacSoundFile(std::unique_ptr<std::istream> file) :
  m_file(std::move(file)),
  m_decoder(FLAC__stream_decoder_new()),
  m_decoded(),
  m_decoded_offset(0)
{
  if (!m_decoder)
    throw SoundError("Couldn't create a FLAC decoder");

  if (FLAC__stream_decoder_init_stream(m_decoder, cb_read, cb_seek, cb_tell, cb_length, cb_eof,
                                       cb_write, cb_metadata, cb_error, this)
        != FLAC__STREAM_DECODER_INIT_STATUS_OK
      || !FLAC__stream_decoder_process_until_end_of_metadata(m_decoder)
      || m_rate == 0)
  {
    FLAC__stream_decoder_delete(m_decoder);
    throw SoundError("file is not a readable FLAC file");
  }
}

FlacSoundFile::~FlacSoundFile()
{
  FLAC__stream_decoder_delete(m_decoder);
}

size_t
FlacSoundFile::read(void* buffer, size_t buffer_size)
{
  char* out = static_cast<char*>(buffer);
  size_t done = 0;

  while (done < buffer_size) {
    const size_t available = m_decoded.size() * sizeof(int16_t) - m_decoded_offset;
    if (available == 0) {
      m_decoded.clear();
      m_decoded_offset = 0;
      if (FLAC__stream_decoder_get_state(m_decoder) == FLAC__STREAM_DECODER_END_OF_STREAM
          || !FLAC__stream_decoder_process_single(m_decoder))
        break;
      continue;
    }

    const size_t bytes = std::min(buffer_size - done, available);
    std::memcpy(out + done, reinterpret_cast<const char*>(m_decoded.data()) + m_decoded_offset, bytes);
    m_decoded_offset += bytes;
    done += bytes;
  }

  return done;
}

void
FlacSoundFile::reset()
{
  m_decoded.clear();
  m_decoded_offset = 0;

  // Rewinds to the start and reads the few bytes of headers again
  FLAC__stream_decoder_reset(m_decoder);
  FLAC__stream_decoder_process_until_end_of_metadata(m_decoder);
}

FLAC__StreamDecoderReadStatus
FlacSoundFile::cb_read(const FLAC__StreamDecoder*, FLAC__byte buffer[], size_t* bytes, void* source)
{
  std::istream& file = *static_cast<FlacSoundFile*>(source)->m_file;
  file.read(reinterpret_cast<char*>(buffer), static_cast<std::streamsize>(*bytes));
  *bytes = static_cast<size_t>(file.gcount());
  // A short read at the end sets eofbit, which would make the next seek fail
  file.clear();
  return (*bytes == 0) ? FLAC__STREAM_DECODER_READ_STATUS_END_OF_STREAM
                       : FLAC__STREAM_DECODER_READ_STATUS_CONTINUE;
}

FLAC__StreamDecoderSeekStatus
FlacSoundFile::cb_seek(const FLAC__StreamDecoder*, FLAC__uint64 offset, void* source)
{
  std::istream& file = *static_cast<FlacSoundFile*>(source)->m_file;
  file.clear();
  if (!file.seekg(static_cast<std::streamoff>(offset), std::ios::beg))
    return FLAC__STREAM_DECODER_SEEK_STATUS_ERROR;
  return FLAC__STREAM_DECODER_SEEK_STATUS_OK;
}

FLAC__StreamDecoderTellStatus
FlacSoundFile::cb_tell(const FLAC__StreamDecoder*, FLAC__uint64* offset, void* source)
{
  std::istream& file = *static_cast<FlacSoundFile*>(source)->m_file;
  const std::streampos pos = file.tellg();
  if (pos < 0)
    return FLAC__STREAM_DECODER_TELL_STATUS_ERROR;
  *offset = static_cast<FLAC__uint64>(pos);
  return FLAC__STREAM_DECODER_TELL_STATUS_OK;
}

FLAC__StreamDecoderLengthStatus
FlacSoundFile::cb_length(const FLAC__StreamDecoder*, FLAC__uint64* length, void* source)
{
  std::istream& file = *static_cast<FlacSoundFile*>(source)->m_file;
  const std::streampos pos = file.tellg();
  file.seekg(0, std::ios::end);
  const std::streampos end = file.tellg();
  file.seekg(pos);
  if (pos < 0 || end < 0)
    return FLAC__STREAM_DECODER_LENGTH_STATUS_ERROR;
  *length = static_cast<FLAC__uint64>(end);
  return FLAC__STREAM_DECODER_LENGTH_STATUS_OK;
}

FLAC__bool
FlacSoundFile::cb_eof(const FLAC__StreamDecoder*, void* source)
{
  std::istream& file = *static_cast<FlacSoundFile*>(source)->m_file;
  const bool at_end = file.peek() == std::char_traits<char>::eof();
  file.clear();
  return at_end;
}

FLAC__StreamDecoderWriteStatus
FlacSoundFile::cb_write(const FLAC__StreamDecoder*, const FLAC__Frame* frame,
                        const FLAC__int32* const samples[], void* source)
{
  auto self = static_cast<FlacSoundFile*>(source);
  const unsigned channels = frame->header.channels;
  const unsigned bits = frame->header.bits_per_sample;

  // Interleaved 16 bit in the machine's byte order, the same as the other loaders hand out
  const size_t start = self->m_decoded.size();
  self->m_decoded.resize(start + frame->header.blocksize * channels);
  int16_t* out = self->m_decoded.data() + start;
  for (unsigned i = 0; i < frame->header.blocksize; ++i) {
    for (unsigned c = 0; c < channels; ++c) {
      const FLAC__int32 sample = samples[c][i];
      *out++ = static_cast<int16_t>(bits > 16 ? sample >> (bits - 16) : sample << (16 - bits));
    }
  }
  return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
}

void
FlacSoundFile::cb_metadata(const FLAC__StreamDecoder*, const FLAC__StreamMetadata* metadata, void* source)
{
  if (metadata->type != FLAC__METADATA_TYPE_STREAMINFO)
    return;

  auto self = static_cast<FlacSoundFile*>(source);
  const FLAC__StreamMetadata_StreamInfo& info = metadata->data.stream_info;
  self->m_channels = static_cast<int>(info.channels);
  self->m_rate = static_cast<int>(info.sample_rate);
  self->m_bits_per_sample = 16;
  self->m_size = static_cast<size_t>(info.total_samples) * info.channels * 2;
}

void
FlacSoundFile::cb_error(const FLAC__StreamDecoder*, FLAC__StreamDecoderErrorStatus status, void*)
{
  log_warning << "FLAC decoding error: " << FLAC__StreamDecoderErrorStatusString[status] << std::endl;
}

/* EOF */
