#pragma once

#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>
#include "converter.h"

namespace commander {

class CommandFileOpenError: public std::runtime_error {
	public:
		CommandFileOpenError();
};

class Commander {
	private:
		std::ifstream _stream;
		const std::vector<std::string>& _paths;
		converter::Factory _create_converter;

		void _command_line(std::string& line) const;
	public:
		Commander(
			const std::string& path,
			const std::vector<std::string>& inputs,
			converter::Factory create_converter
		);
		~Commander() = default;
		void command();
};

class InputPathsNotUniqueError: public std::invalid_argument {
	public:
		InputPathsNotUniqueError();
};

class InvalidFileIndex: public std::invalid_argument {
	public:
		InvalidFileIndex(size_t index);
};

class InvalidNumber: public std::invalid_argument {
	public:
		InvalidNumber();
};

class NoConverterName: public std::invalid_argument {
	public:
		NoConverterName();
};

class NoInputPaths: public std::invalid_argument {
	public:
		NoInputPaths();
};

}
