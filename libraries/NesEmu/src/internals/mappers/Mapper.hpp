#ifndef EMU_MAPPER_HEADER
#define EMU_MAPPER_HEADER

#include "internals/Core.hpp"

#include <varf/FileManager.hpp>
#include <varf/Serializable.hpp>

#include <optional>
#include <string>

namespace Emu
{
class IMapper : public varf::ISerializable
{
public:
	IMapper(const u8 prgBanks, const u8 chrBanks) : m_prgBanks(prgBanks), m_chrBanks(chrBanks)
	{
	}
	virtual ~IMapper() = default;

	[[nodiscard]] virtual std::optional<u16> CpuMapRead(const u16 addr) const = 0;
	[[nodiscard]] virtual std::optional<u16> CpuMapWrite(const u16 addr) const = 0;

	[[nodiscard]] virtual std::optional<u16> PpuMapRead(const u16 addr) const = 0;
	[[nodiscard]] virtual std::optional<u16> PpuMapWrite(const u16 addr) const = 0;

	virtual std::string GetName() = 0;
	// Inherited via ISerializable
	virtual void Serialize(std::ostream& fs) override = 0;
	virtual void Deserialize(std::istream& fs) override = 0;
protected:
	u8 m_prgBanks;
	u8 m_chrBanks;

};
} // namespace Emu



#endif