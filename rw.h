#pragma once

#include <fstream>
#include <memory>
#include "header.h"

namespace rw {

class DoesNotExist: public std::runtime_error {
	public:
		DoesNotExist(const std::string& path);
};

class Rw {
	protected:
		std::fstream _stream;

	public:
		Rw(const std::string& path);

		virtual ~Rw() = default;
		virtual std::unique_ptr<header::Header> read_header() = 0;
		virtual std::int64_t read_sample(size_t position) = 0;
		virtual void write_header(const header::Header* header) = 0;
		virtual void write_sample(size_t position, std::int64_t sample) = 0;
};

}
