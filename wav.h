#pragma once

#include <cstdint>
#include <stdexcept>
#include <string>
#include "header.h"

namespace wav {

// Riff chunk
const char RIFF_CHUNK_NAME[4] = {'R', 'I', 'F', 'F'};
const char _RIFF_FORMAT_NAME[4] = {'W', 'A', 'V', 'E'};
const size_t RIFF_CHUNK_POSITION = 8;
const size_t RIFF_CHUNK_SIZE =
	sizeof(char[4]) + sizeof(std::uint32_t) + sizeof(char[4]);

// Fmt chunk
const char FMT_CHUNK_NAME[4] = {'f', 'm', 't', ' '};
const size_t FMT_CHUNK_POSITION = RIFF_CHUNK_POSITION + RIFF_CHUNK_SIZE;
const size_t FMT_CHUNK_SIZE =
	sizeof(char[4])
	+ sizeof(std::uint32_t)
	+ sizeof(std::uint16_t)
	+ sizeof(std::uint16_t)
	+ sizeof(std::uint32_t)
	+ sizeof(std::uint32_t)
	+ sizeof(std::uint16_t)
	+ sizeof(std::uint16_t);
const std::uint32_t _CHUNK_SIZE = 16;
const std::uint16_t _AUDIO_FMT = 1;
const std::uint32_t _CHANNELS_COUNT = 1;
const std::uint32_t _SAMPLES_PER_SECOND = 44100;
const std::uint32_t _BYTES_PER_SECOND = 88200;
const std::uint32_t _SAMPLE_SIZE = 2;
const std::uint32_t _BITS_PER_SAMPLE = 16;

// Data chunk
const char DATA_CHUNK_NAME[4] = {'d', 'a', 't', 'a'};
const size_t DATA_CHUNK_POSITION = FMT_CHUNK_POSITION + FMT_CHUNK_SIZE;
const size_t DATA_CHUNK_SIZE = sizeof(char[4]) + sizeof(std::uint32_t);

// All chunks
const size_t CHUNKS_SIZE = RIFF_CHUNK_SIZE + FMT_CHUNK_SIZE + DATA_CHUNK_SIZE;

class Header: public header::Header {
	private:
		// Contains "RIFF"
		char _riff_chunk_name[4];
		// Size of wav portion, which follows the first 8 bytes
		std::uint32_t _size;
		// Contains "WAVE"
		char _riff_format_name[4];

		// Contains "fmt"
		char _fmt_chunk_name[4];
		// For example, 16 for PCM
		std::uint32_t _chunk_size;
		// For example, 1 for PCM
		std::uint16_t _audio_fmt;
		std::uint16_t _channels_count;
		// Samples count in one second
		std::uint32_t _samples_per_second;
		// Bytes per second:
		// `(samples_per_second * bits_per_sample * channels_count) / 8`
		std::uint32_t _bytes_per_second;
		// Sample size: `(channels_count * bits_per_sample) / 8`
		std::uint16_t _sample_size;
		std::uint16_t _bits_per_sample;

		// Contains "data"
		char _data_chunk_name[4];
		// `samples_count * channels_count * bits_per_sample / 8`
		std::uint32_t _data_size;

	public:
		size_t samples_count() const noexcept override;
		void samples_count(size_t value) noexcept override;
		size_t second_to_sample(size_t second) const noexcept override;
		void remove_unused_chunks_from_size() noexcept;
		void validate() const;
};

class InvalidHeader: public std::invalid_argument {
	public:
		InvalidHeader(std::string_view field);
};

}
