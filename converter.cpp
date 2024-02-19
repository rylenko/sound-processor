#include "converter.h"

namespace converter {

class InvalidArgs: public Error {
	public:
		InvalidArgs(std::string_view name, size_t rws, size_t args);
};

InvalidArgs::InvalidArgs(std::string_view name, size_t rws, size_t args)
: Error(
	std::string("Invalid args: ")
	+ std::string(name)
	+ std::string(" with ")
	+ std::to_string(rws)
	+ std::string(" files and ")
	+ std::to_string(args)
	+ std::string(" args.")
) {}

class Mix: public Converter {
	private:
		rw::Rw* _rw1;
		rw::Rw* _rw2;
		size_t _from;

	public:
		Mix(rw::Rw* _rw1, rw::Rw* _rw2, size_t from) noexcept;
		void convert() override;
};

Mix::Mix(rw::Rw* rw1, rw::Rw* rw2, size_t from)
noexcept: _rw1(rw1), _rw2(rw2), _from(from) {}

void Mix::convert() {
	// Get headers
	std::unique_ptr<header::Header> header1 = this->_rw1->read_header();
	std::unique_ptr<header::Header> header2 = this->_rw2->read_header();

	// Get size info
	size_t skip_samples = header2->second_to_sample(this->_from);
	size_t mixed_samples_count = header2->samples_count() - skip_samples;
	size_t old_samples_count = header1->samples_count();

	// Increase sizes and write a new header
	if (mixed_samples_count > old_samples_count) {
		header1->samples_count(mixed_samples_count);
		this->_rw1->write_header(header1.get());
	}

	// Set position and mix bytes
	for (size_t i = 0; i < mixed_samples_count; ++i) {
		std::int64_t sample = this->_rw2->read_sample(skip_samples + i);
		if (i < old_samples_count) {
			sample = (sample + this->_rw1->read_sample(i)) / 2;
		}
		this->_rw1->write_sample(i, sample);
	}
}

class Mute: public Converter {
	private:
		rw::Rw* _rw;
		size_t _from;
		size_t _to;

	public:
		Mute(rw::Rw* rw, size_t from, size_t to) noexcept;
		void convert() override;
};

Mute::Mute(rw::Rw* rw, size_t from, size_t to)
noexcept: _rw(rw), _from(from), _to(to) {}

void Mute::convert() {
	std::unique_ptr<header::Header> header = this->_rw->read_header();
	size_t from_sample = header->second_to_sample(this->_from);
	size_t to_sample = header->second_to_sample(this->_to + 1);
	for (size_t i = from_sample; i < to_sample; ++i) {
		this->_rw->write_sample(i, 0);
	}
}

class SetVolume: public Converter {
	private:
		rw::Rw* _rw;
		size_t _from;
		size_t _to;
		double _factor;

	public:
		SetVolume(rw::Rw* rw, size_t from, size_t to, size_t percents);
		void convert() override;
};

SetVolume::SetVolume(
	rw::Rw* rw,
	size_t from,
	size_t to,
	size_t percents
): _rw(rw), _from(from), _to(to) {
	this->_factor = static_cast<double>(percents) / 100;
	this->_factor *= this->_factor;
}

void SetVolume::convert() {
	std::unique_ptr<header::Header> header = this->_rw->read_header();
	size_t from_sample = header->second_to_sample(this->_from);
	size_t to_sample = header->second_to_sample(this->_to + 1);
	for (size_t i = from_sample; i < to_sample; ++i) {
		this->_rw->write_sample(i, this->_rw->read_sample(i) * this->_factor);
	}
}

std::unique_ptr<Converter> _create(
	std::string_view name,
	const std::vector<std::unique_ptr<rw::Rw>>& rws,
	const std::vector<size_t>& values
) {
	Converter* converter;

	// Get the converter
	if (name.compare("mix") == 0 && rws.size() == 2 && values.size() == 1) {
		converter = new Mix(rws[0].get(), rws[1].get(), values[0]);
	} else if (
		name.compare("mute") == 0
		&& rws.size() == 1
		&& values.size() == 2
	) {
		converter = new Mute(rws[0].get(), values[0], values[1]);
	} else if (
		name.compare("set_volume") == 0
		&& rws.size() == 1
		&& values.size() == 3
	) {
		converter = new SetVolume(rws[0].get(), values[0], values[1], values[2]);
	} else {
		throw InvalidArgs(name, rws.size(), values.size());
	}

	return std::unique_ptr<Converter>(converter);
}

Factory create = _create;

}
