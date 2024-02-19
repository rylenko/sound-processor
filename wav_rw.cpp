#include <algorithm>
#include <cstring>
#include "wav_rw.h"
#include "wav.h"

namespace wav_rw {

ReadChunkNameError::ReadChunkNameError()
: std::runtime_error("Failed to read chunk's name.") {}

ReadDataChunkError::ReadDataChunkError()
: std::runtime_error("Failed to read the data chunk.") {}

ReadFmtChunkError::ReadFmtChunkError()
: std::runtime_error("Failed to read the fmt chunk.") {}

ReadRiffChunkError::ReadRiffChunkError()
: std::runtime_error("Failed to read the riff chunk.") {}

Rw::Rw(const std::string& path): rw::Rw(path) {}

std::unique_ptr<header::Header> Rw::read_header() {
	// Go to the beginning of the file
	if (!this->_stream.seekg(0)) {
		throw SeekError("beginning");
	}

	// Create header and read riff chunk
	wav::Header header;
	char* header_char = reinterpret_cast<char*>(&header);
	char* riff_position = header_char + wav::RIFF_CHUNK_POSITION;
	if (!this->_stream.read(riff_position, wav::RIFF_CHUNK_SIZE)) {
		throw ReadRiffChunkError();
	}

	// Readed marks and buffer for next chunks
	bool fmt_chunk_readed = false;
	bool data_chunk_readed = false;
	char chunk_name[4] = {0};

	// Read other chunks
	while (!(fmt_chunk_readed && data_chunk_readed) && !this->_stream.eof()) {
		// Read next chunk name
		std::copy_n(chunk_name + 1, sizeof(chunk_name) - 1, chunk_name);
		if (!this->_stream.read(&chunk_name[sizeof(chunk_name) - 1], sizeof(char))) {
			throw ReadChunkNameError();
		}

		if (std::memcmp(chunk_name, wav::FMT_CHUNK_NAME, sizeof(chunk_name)) == 0) {
			// Mark as readed and go back
			fmt_chunk_readed = true;
			if (!this->_stream.seekg(-sizeof(chunk_name), std::ios::cur)) {
				throw SeekError("fmt chunk");
			}
			// Read chunk
			char* fmt_position = header_char + wav::FMT_CHUNK_POSITION;
			if (!this->_stream.read(fmt_position, wav::FMT_CHUNK_SIZE)) {
				throw ReadFmtChunkError();
			}
		} else if (
			std::memcmp(chunk_name, wav::DATA_CHUNK_NAME, sizeof(chunk_name)) == 0
		) {
			// Mark as readed and go back
			data_chunk_readed = true;
			if (!this->_stream.seekg(-sizeof(chunk_name), std::ios::cur)) {
				throw SeekError("data chunk");
			}
			// Read chunk
			char* data_position = header_char + wav::DATA_CHUNK_POSITION;
			if (!this->_stream.read(data_position, wav::DATA_CHUNK_SIZE)) {
				throw ReadDataChunkError();
			}
		}
	}

	// Validate header and remove from size other chunks size
	header.validate();
	header.remove_unused_chunks_from_size();

	return std::unique_ptr<header::Header>(new wav::Header(header));
}

std::int64_t Rw::read_sample(size_t position) {
	// Go to the position of sample
	size_t data_position = position * sizeof(std::int16_t);
	if (!this->_stream.seekg(wav::CHUNKS_SIZE + data_position)) {
		throw SeekError(std::string("read position ") + std::to_string(position));
	}

	// Read the sample
	return static_cast<std::int16_t>(
		this->_stream.get() | this->_stream.get() << 8
	);
}

void Rw::write_header(const header::Header* header) {
	// Go to beginning of the file and write
	if (!this->_stream.seekp(0)) {
		throw SeekError("beginning.");
	}
	const char* position = reinterpret_cast<const char*>(header)
		+ wav::RIFF_CHUNK_POSITION;
	if (!this->_stream.write(position, wav::CHUNKS_SIZE)) {
		throw WriteHeaderError();
	}
}

void Rw::write_sample(size_t position, std::int64_t sample) {
	// Go to the sample position
	size_t data_position = position * sizeof(std::int16_t);
	if (!this->_stream.seekp(wav::CHUNKS_SIZE + data_position)) {
		throw SeekError(std::string("write position ") + std::to_string(position));
	}

	// Write sample
	std::int16_t value = std::clamp(sample, this->_MIN_SAMPLE, this->_MAX_SAMPLE);
	this->_stream.put(value);
	this->_stream.put(value >> 8);
}

SeekError::SeekError(std::string_view to): std::runtime_error(
	"Failed to seek to the " + std::string(to) + " of file."
) {}

WriteHeaderError::WriteHeaderError():
std::runtime_error("Failed to write a header.") {}

}
