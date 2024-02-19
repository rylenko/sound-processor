#include <cstring>
#include "wav.h"

namespace wav {

size_t Header::samples_count() const noexcept {
	return this->_data_size / this->_sample_size;
}

void Header::samples_count(size_t value) noexcept {
	this->_size += value * this->_sample_size - this->_data_size;
	this->_data_size = value * this->_sample_size;
}

void Header::remove_unused_chunks_from_size() noexcept {
	this->_size = CHUNKS_SIZE + this->_data_size;
}

size_t Header::second_to_sample(size_t second) const noexcept {
	return CHUNKS_SIZE + this->_samples_per_second * second;
}

void Header::validate() const {
	if (std::memcmp(
		this->_riff_chunk_name, RIFF_CHUNK_NAME, sizeof(RIFF_CHUNK_NAME) != 0
	)) {
		throw InvalidHeader("riff chunk name");
	} else if (std::memcmp(
		this->_riff_format_name, _RIFF_FORMAT_NAME, sizeof(_RIFF_FORMAT_NAME) != 0
	)) {
		throw InvalidHeader("riff format name");
	} else if (std::memcmp(
		this->_fmt_chunk_name, FMT_CHUNK_NAME, sizeof(FMT_CHUNK_NAME) != 0
	)) {
		throw InvalidHeader("fmt chunk name");
	} else if (this->_chunk_size != _CHUNK_SIZE) {
		throw InvalidHeader("chunk size");
	} else if (this->_audio_fmt != _AUDIO_FMT) {
		throw InvalidHeader("audio fmt");
	} else if (this->_channels_count != _CHANNELS_COUNT) {
		throw InvalidHeader("channels count");
	} else if (this->_samples_per_second != _SAMPLES_PER_SECOND) {
		throw InvalidHeader("samples per second");
	} else if (this->_bytes_per_second != _BYTES_PER_SECOND) {
		throw InvalidHeader("bytes per second");
	} else if (this->_sample_size != _SAMPLE_SIZE) {
		throw InvalidHeader("sample size");
	} else if (this->_bits_per_sample != _BITS_PER_SAMPLE) {
		throw InvalidHeader("bits per sample");
	} else if (std::memcmp(
		this->_data_chunk_name, DATA_CHUNK_NAME, sizeof(DATA_CHUNK_NAME) != 0
	)) {
		throw InvalidHeader("data chunk name");
	} else if (this->_data_size == 0) {
		throw InvalidHeader("data size");
	}
}

InvalidHeader::InvalidHeader(std::string_view field): std::invalid_argument(
	std::string("Invalid header's ") + std::string(field)
) {}

}
