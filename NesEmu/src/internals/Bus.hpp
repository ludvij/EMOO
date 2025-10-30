#ifndef EMU_BUS_HEADER
#define EMU_BUS_HEADER

#include "APU.hpp"
#include "Core.hpp"
#include "FileManager/FileManager.hpp"
#include "PPU.hpp"

#include <array>
#include <memory>

namespace A6502
{
class Disassembler;
}

namespace Emu
{
class Cartridge;
class Controller;

/*
 * This class is an abstraction for the cpu memory, separated from the cpu class
 * since everyone and their mother has to access it somehow
 */
class Bus : public Fman::ISerializable
{
public:
	u8 Read(u16 addr) const;
	u8 Peek(u16 addr) const;
	void Write(u16 addr, u8 val);

	void ConnectCartridge(std::shared_ptr<Cartridge> cartridge)
	{
		m_cartridge = cartridge;
	}
	void ConnectPPU(PPU* ppu)
	{
		m_ppu = ppu;
	}

	void DMA(u64 cycles);

	void ConnectAPU(APU* apu)
	{
		m_apu = apu;
	}

	void ConnectController(u8 port, Controller* controller)
	{
		m_controller[port] = controller;
	}

	void Reset();

	// Inherited via ISerializable
	void Serialize(std::ostream& fs) override;
	void Deserialize(std::istream& fs) override;

private:
	// maybe do this in a single array in the form of
	// 0x4020 to reach everything until the cartridge
	// the problem is that this approach wastes a ton of memory
	// 0x3020 bytes to be exact
	// std::array<u8, 0x4020> m_memory{0};
	std::array<u8, 0x800> m_cpu_ram{ 0 };
	std::shared_ptr<Cartridge> m_cartridge{ nullptr };
	PPU* m_ppu = nullptr;
	APU* m_apu = nullptr;

	Controller* m_controller[2] = { nullptr, nullptr };

	friend class A6502::Disassembler;

};

}

#endif