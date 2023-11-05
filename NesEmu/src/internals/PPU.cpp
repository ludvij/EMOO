#include "pch.hpp"

#include "PPU.hpp"

#include "Bus.hpp"

namespace Emu
{
void PPU::ConnectBus(Bus *bus)
{
	m_bus = bus;
	PPUCTRL.Link(bus);
	PPUMASK.Link(bus);
	PPUSCROLL.Link(bus);
	OAMADDR.Link(bus);
	OAMDATA.Link(bus);
	PPUSCROLL.Link(bus);
	PPUADDR.Link(bus);
	PPUDATA.Link(bus);
	OAMDMA.Link(bus);
}

u8 PPU::memoryRead(u16 addr)
{
	return m_bus->Read(addr);
}

void PPU::memoryWrite(u16 addr, u8 val)
{
	m_bus->Write(addr, val);
}

}
