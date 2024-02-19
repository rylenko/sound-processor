#include <filesystem>
#include <stdexcept>
#include "rw.h"

namespace rw {

DoesNotExist::DoesNotExist(const std::string& path)
: std::runtime_error(path + std::string(" doesn't exist.")) {}

Rw::Rw(const std::string& path) {
	if (!std::filesystem::exists(path)) {
		throw DoesNotExist(path);
	}
	this->_stream = std::fstream(
		path,
		std::ios::in | std::ios::out | std::ios::binary | std::ios::ate
	);
}

}
