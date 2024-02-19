#pragma once

namespace header {

class Header {
	public:
		virtual ~Header() = default;
		virtual size_t samples_count() const noexcept = 0;
		virtual void samples_count(size_t value) noexcept = 0;
		virtual size_t second_to_sample(size_t second) const noexcept = 0;
};

}
