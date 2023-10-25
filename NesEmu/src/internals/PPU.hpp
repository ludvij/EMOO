#pragma once

#include "Core.hpp"

namespace Emu
{

constexpr u32 NTSC_FRAMERATE = 60;
constexpr u32 PAL_FRAMERATE  = 50;

class PPU
{
public:
	void ConnectBus(Bus* bus) noexcept {m_bus = bus;}
private:
	u8 memoryRead(u16 addr);
	void memoryWrite(u16 addr, u8 val);
private:
	Bus* m_bus = nullptr;;
};

}
