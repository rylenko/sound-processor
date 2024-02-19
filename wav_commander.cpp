#include <cstring>
#include <numeric>
#include "wav_commander.h"
#include "wav_rw.h"

namespace commander {

CommandFileOpenError::CommandFileOpenError()
: std::runtime_error("Failed to open command file.") {}

Commander::Commander(
	const std::string& path,
	const std::vector<std::string>& inputs,
	converter::Factory create_converter
): _paths(inputs), _create_converter(create_converter) {
	// Check inputs count
	if (inputs.size() == 0) {
		throw NoInputPaths();
	}

	// Check there are no equal elements
    auto unique_elements = 1 + std::inner_product(
		std::next(inputs.begin()),
		inputs.end(),
		inputs.begin(),
		size_t(0),
		std::plus<size_t>(),
		std::not_equal_to<const std::string&>()
	);
	if (unique_elements != inputs.size()) {
		throw InputPathsNotUniqueError();
	}

	// Open command file
	this->_stream = std::ifstream(path);
	if (!this->_stream) {
		throw CommandFileOpenError();
	}
}

void Commander::command() {
	for (std::string line; std::getline(this->_stream, line);) {
		this->_command_line(line);
	}
}

void Commander::_command_line(std::string& line) const {
	char* str = const_cast<char*>(line.c_str());

	// Read a name
	const char* name = std::strtok(str, " ");
	if (!name) {
		throw NoConverterName();
	}

	// Args
	std::vector<std::unique_ptr<rw::Rw>> rws;
	std::vector<size_t> values;

	// Read args
	const char* arg_str = std::strtok(NULL, " ");
	while (arg_str != NULL) {
		if (arg_str[0] == '$') {
			// Add a new rw
			size_t index = std::strtoul(arg_str + 1, NULL, 10);
			if (index >= this->_paths.size()) {
				throw InvalidFileIndex(index);
			}
			rws.push_back(std::unique_ptr<rw::Rw>(new wav_rw::Rw(this->_paths[index])));
		} else {
			// Add new value
			values.push_back(std::strtoul(arg_str, NULL, 10));
			if (errno == ERANGE) {
				throw InvalidNumber();
			}
		}

		arg_str = std::strtok(NULL, " ");
	}

	this->_create_converter(name, rws, values)->convert();
}

InputPathsNotUniqueError::InputPathsNotUniqueError()
: std::invalid_argument("Input paths aren't unique.") {}

InvalidFileIndex::InvalidFileIndex(size_t index): std::invalid_argument(
	std::string("Invalid file index: ") + std::to_string(index)
) {}

InvalidNumber::InvalidNumber()
: std::invalid_argument("Failed to parse a number.") {}

NoConverterName::NoConverterName()
: std::invalid_argument("There is no converter name.") {}

NoInputPaths::NoInputPaths()
: std::invalid_argument("There is no input files.") {}

}
