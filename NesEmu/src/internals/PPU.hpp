#ifndef EMU_PPU_HEADER
#define EMU_PPU_HEADER

#include "Core.hpp"
#include "Bus.hpp"

namespace Emu
{

constexpr u32 NTSC_FRAMERATE = 60;
constexpr u32 PAL_FRAMERATE  = 50;

template<u16 address>
struct MemoryMappedRegister
{
	void Link(Bus* bus) { m_bus = bus; }

	u8 Get() const
	{ 
		#ifdef NES_EMU_DEBUG
		if (m_bus == nullptr) 
			std::throw_with_nested(std::runtime_error("Memory mapped register was not mapped to a bus"));
		#endif
		return m_bus->Read(address);
	}
	void Set(const u8 val)
	{
		#ifdef NES_EMU_DEBUG
		if (m_bus == nullptr) 
			std::throw_with_nested(std::runtime_error("Memory mapped register was not mapped to a bus"));
		#endif
		m_bus->Write(address, val);
	}
	u8 operator()() const         { return Get(); }
	void operator()(const u8 val) { Set(val);     }

	// I don't want to reassign this, ever
	MemoryMappedRegister(const MemoryMappedRegister&)            = delete;
	MemoryMappedRegister& operator=(const MemoryMappedRegister&) = delete;
private:
	Bus* m_bus = nullptr;
};


class PPU
{
public:
	void ConnectBus(Bus* bus);

public:
	MemoryMappedRegister<0x2000> PPUCTRL;
	MemoryMappedRegister<0x2001> PPUMASK;
	MemoryMappedRegister<0x2002> PPUSTATUS;
	MemoryMappedRegister<0x2003> OAMADDR;
	MemoryMappedRegister<0x2004> OAMDATA;
	MemoryMappedRegister<0x2005> PPUSCROLL;
	MemoryMappedRegister<0x2006> PPUADDR;
	MemoryMappedRegister<0x2007> PPUDATA;
	MemoryMappedRegister<0x4014> OAMDMA;
private:
	u8 memoryRead(u16 addr);
	void memoryWrite(u16 addr, u8 val);
private:
	Bus* m_bus = nullptr;
};

}

#endif