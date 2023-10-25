#pragma once

namespace Emu
{
/*
 * https://www.nesdev.org/wiki/NES_2.0 
 * This struct contains the full nes2.0 spec
 * Even tho i won't use half its features since i don't care for arcade
 * or unofficial nes versions like the dendy
 * but I will be using the pgr and chr rom expansions
 * 
 * I won't use the mapper extension or submapper, but I want
 * to have implemented the system to be able to have them in the future just in case
 * 
 * I don't care about miscellaneous nor expansion devices
 * 
 * I also don't really know about vollatile ram, will have to check that out
 */
struct iNesHeader
{
	std::array<uint8_t, 4> name;
	/*
	 * number of 16K pgr rom chunks
	 */ 
	uint8_t pgrRamChunks; // byte 4
	/*
	 * number of 8k chr rom chunks if 0 rom uses pgr ram
	 */
	uint8_t chrRomChunks; // byte 5
	/* 
	 * NNNN FTBM
	 * N Mapper numer D0..D3
	 * F four screen mode. 0 = no, 1 = yes (if set, M byte has no effect)
	 * T trainer. 0 = no trainer, 1 = trainer present (512 bytes at $7000-$71FF)
	 * M mirroring. 0 = horizontal, 1 = vertical
	 */
	uint8_t mapper0; // byte 6
	/*
	 * NNNN 10PV
	 * N mapper number D4..D7
	 * P Playchoice 10. When set, this is a pc-10 game
	 * V Vs. Unysystem. When set, this is a Vs. game
	 * bits 2 and 3 represent the validity of the NES 2.0 header, if not correct
	 * a iNes file will be loaded instead
	 */
	uint8_t mapper1; // byte 7
	/*
	 * SSSS NNNN
	 * S submapper number
	 * N mapper number D8..D11
	 */
	uint8_t subMapper; // byte 8
	/*
	 * CCCC PPPP
	 * C 4 more chr rom bits
	 * P 4 more pgr rom bits
	 */ 
	uint8_t romExt; // byte 9
	/*
	 * pppp PPPP
	 * p quantity of pgr RAM that is battery packed
	 * P quantity of pgr RAM that is not battery packed
	 */
	uint8_t pgrRamSize; // byte 10
	/*
	 * cccc CCCC
	 * c quantity of chr RAM that is battery packed
	 * C quantity of chr RAM that is not battery packed
	 */ 
	uint8_t chrRamSize; // byte 11
	/*
	 * ____ __VV
	 * V cpu/ppu timing mode
	 *				0 : ntsc
	 *				1 : pal
	 *				2 : multiple
	 *				3 : dendy
	 */
	uint8_t timing; // byte 12
	/*
	 * MMMM PPPP
	 * M vs. hardware
	 * P vs. ppu
	 */
	uint8_t vsSytemType; // byte 13
	/*
	 * ____ __RR
	 * R number of miscellaneous roms present
	 */ 
	uint8_t miscellaneous; // byte 14
	/*
	 * __DD DDDD
	 * D Default expansion device
	 */
	uint8_t expansionDevice; // byte 15

};
class Mapper;
class Cartridge
{
public:
	Cartridge(std::string_view filePath);
private:
	bool readHeader();

	void readPGRChunks();

private:
	iNesHeader m_header;
	Mapper* m_mapper;

	u32 m_pgrRomChunks;
};
}


