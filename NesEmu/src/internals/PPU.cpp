#include "pch.hpp"

#include "PPU.hpp"

namespace Emu
{

u8 PPU::memoryRead(u16 addr)
{
	return m_bus->Read(addr);
}

void PPU::memoryWrite(u16 addr, u8 val)
{
	m_bus->Write(addr, val);
}

}
