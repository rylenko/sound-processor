#pragma once

#include <limits>
#include "rw.h"

namespace wav_rw {

class ReadChunkNameError: public std::runtime_error {
	public:
		ReadChunkNameError();
};

class ReadDataChunkError: public std::runtime_error {
	public:
		ReadDataChunkError();
};

class ReadFmtChunkError: public std::runtime_error {
	public:
		ReadFmtChunkError();
};

class ReadRiffChunkError: public std::runtime_error {
	public:
		ReadRiffChunkError();
};

class Rw: public rw::Rw {
	private:
		constexpr std::int64_t static const _MIN_SAMPLE =
			std::numeric_limits<std::int16_t>::min();
		constexpr std::int64_t static const _MAX_SAMPLE =
			std::numeric_limits<std::int16_t>::max();

	public:
		Rw(const std::string& path);
		std::unique_ptr<header::Header> read_header() override;
		std::int64_t read_sample(size_t position) override;
		void write_header(const header::Header* header) override;
		void write_sample(size_t position, std::int64_t sample) override;
};

class SeekError: public std::runtime_error {
	public:
		SeekError(std::string_view to);
};

class WriteHeaderError: public std::runtime_error {
	public:
		WriteHeaderError();
};

}
