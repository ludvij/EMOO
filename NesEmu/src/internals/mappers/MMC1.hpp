#ifndef EMU_MMC1_HEADER
#define EMU_MMC1_HEADER

#include "FileManager/FileManager.hpp"
#include "internals/Core.hpp"
#include "Mapper.hpp"

namespace Emu
{
/*
* mapper 001 (MMC1)
*
* CHR bank 1 ($6000-$7FFF) optional
* CHR bank 2 ($8000-$BFFF) switchable or fixed to 1
* CHR bank 3 ($C000-$7FFF) switchable or fixed to 3
*
* PRG bank 1 ($0000-$0FFF) switchable
* PRG bank 2 ($1000-$1FFF) switchable
*/
class MMC1 : public IMapper
{
public:
	MMC1(u8 prgBanks, u8 chrBanks);

	virtual std::optional<u16> CpuMapRead(const u16 addr) const override;
	virtual std::optional<u16> CpuMapWrite(const u16 addr) const override;
	virtual std::optional<u16> PpuMapRead(const u16 addr) const override;
	virtual std::optional<u16> PpuMapWrite(const u16 addr) const override;



	// Inherited via IMapper
	virtual std::string GetName() override;


	// Inherited via IMapper
	void Serialize(std::fstream& fs) override;

	void Deserialize(std::fstream& fs) override;

private:

};

} // namespace Emu

#endif