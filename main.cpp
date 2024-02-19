#include <cstring>
#include <iostream>
#include "wav.h"
#include "wav_commander.h"
#include "wav_rw.h"

const char* HELP =
	"Arguments: <command-file> <input-file-1 (master)> ... <input-file-N>\n\n"
	"Command file example. Mix 1st input to master input from 120th second,"
	" mute master from 120th to 180th seconds and set master's volume to 50%"
	" from 180th to 200th seconds:\n\n"
	"--------------------------\n"
	"mix $1 $0 120\n"
	"mute $0 120 180\n"
	"set_volume $0 180 200 50%\n"
	"--------------------------\n";

int main(int argc, const char** argv) {
	// Print help
	if (argc <= 1 || strcmp(argv[1], "-h") == 0) {
		std::cout << HELP << std::endl;
		return EXIT_SUCCESS;
	}

	// Args to strings
	std::string commands_path = std::string(argv[1]);
	std::vector<std::string> input_paths;
	input_paths.reserve(argc - 2);
	for (int i = 2; i < argc; ++i) {
		input_paths.push_back(std::string(argv[i]));
	}

	// Command
	try {
		commander::Commander(
			commands_path,
			input_paths,
			converter::create
		).command();
	} catch (const commander::CommandFileOpenError& e) {
		std::cerr << e.what() << std::endl;
		return 1;
	} catch (const commander::InvalidFileIndex& e) {
		std::cerr << e.what() << std::endl;
		return 2;
	} catch (const commander::InvalidNumber& e) {
		std::cerr << e.what() << std::endl;
		return 3;
	} catch (const commander::NoConverterName& e) {
		std::cerr << e.what() << std::endl;
		return 4;
	} catch (const commander::NoInputPaths& e) {
		std::cerr
			<< e.what() << std::endl << std::endl
			<< "--------------------------" << std::endl << std::endl
			<< HELP << std::endl;
		return 5;
	} catch (const wav_rw::ReadChunkNameError& e) {
		std::cerr << e.what() << std::endl;
		return 6;
	} catch (const wav_rw::ReadDataChunkError& e) {
		std::cerr << e.what() << std::endl;
		return 7;
	} catch (const wav_rw::ReadFmtChunkError& e) {
		std::cerr << e.what() << std::endl;
		return 8;
	} catch (const wav_rw::ReadRiffChunkError& e) {
		std::cerr << e.what() << std::endl;
		return 9;
	} catch (const wav::InvalidHeader& e) {
		std::cerr << e.what() << std::endl;
		return 10;
	} catch (const wav_rw::SeekError& e) {
		std::cerr << e.what() << std::endl;
		return 11;
	} catch (const wav_rw::WriteHeaderError& e) {
		std::cerr << e.what() << std::endl;
		return 12;
	} catch (const commander::InputPathsNotUniqueError& e) {
		std::cerr << e.what() << std::endl;
		return 13;
	} catch (const rw::DoesNotExist& e) {
		std::cerr << e.what() << std::endl;
		return 100;
	} catch (const converter::Error& e) {
		std::cerr << e.what() << std::endl;
		return 200;
	}

	return EXIT_SUCCESS;
}
