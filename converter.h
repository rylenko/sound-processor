#pragma once

#include <string>
#include <vector>
#include "rw.h"

namespace converter {

class Converter {
	public:
		virtual ~Converter() = default;
		virtual void convert() = 0;
};

class Error: public std::runtime_error {
	using std::runtime_error::runtime_error;
};

typedef std::unique_ptr<Converter> (*Factory)(
	std::string_view name,
	const std::vector<std::unique_ptr<rw::Rw>>& rws,
	const std::vector<size_t>& args
);

extern Factory create;

}
