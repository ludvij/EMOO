#ifndef EMU_NROM_HEADER
#define EMU_NROM_HEADER

#include "FileManager/FileManager.hpp"
#include "internals/Core.hpp"
#include "Mapper.hpp"

namespace Emu
{
/*
 * mapper 000 (NROM)
 *
 */
class NROM : public IMapper
{
public:
	NROM(u8 prgBanks, u8 chrBanks);

	virtual std::optional<u16> CpuMapRead(const u16 addr) const override;
	virtual std::optional<u16> CpuMapWrite(const u16 addr) const override;
	virtual std::optional<u16> PpuMapRead(const u16 addr) const override;
	virtual std::optional<u16> PpuMapWrite(const u16 addr) const override;



	// Inherited via IMapper
	virtual std::string GetName() override;


	// Inherited via IMapper
	void Serialize(std::fstream& fs) override;

	void Deserialize(std::fstream& fs) override;

};
} // namespace Emu

#endif