#ifndef GRAPHICS_UUID_HEADER
#define GRAPHICS_UUID_HEADER

#include <cstdint>
#include <string>

namespace Ui
{
class UUID
{
public:
	UUID();
	~UUID();

	std::strong_ordering operator<=>(const UUID& other) const = default;

	friend std::string to_string(const UUID& id);

private:
	uint64_t m_lo;
	uint64_t m_hi;
};

using uuid_t = UUID;
}

#endif