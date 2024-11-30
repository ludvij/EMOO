#include "pch.hpp"
#include "Bus.hpp"

namespace Emu
{

u8 Bus::Read(const u16 addr) const
{

	if (addr < 0x2000) // Ram and ram mirrors
	{
		return m_cpu_ram[addr & 0x07FF];
	}
	if (addr >= 0x2000 && addr < 0x4000) // PPU registers and mirrors
	{
		return m_ppu->CpuRead(addr & 0x0007 + 0x2000);
	}
	if (addr >= 0x4000 && addr < 0x4018) // APU and IO functionality
	{
		if (addr == 0x4016 || addr == 0x4017) // JOY1 nad JOY2
		{
			return m_controller[addr & 0x0001]->Read();
		}
		if (addr <= 0x4013 || addr == 0x4015 || addr == 0x4017)
		{
			return m_apu->CpuRead(addr);
		}
		return 0;
	}
	if (addr >= 0x4018 && addr < 0x4020) // APU and IO functionality Test mode
	{
		return 0;
	}
	else // cartridge space
	{
		return *m_cartridge->CpuRead(addr);

	}

}

u8 Bus::Peek(u16 addr) const
{
	if (addr < 0x2000) // Ram and ram mirrors
	{
		return m_cpu_ram[addr & 0x07FF];
	}
	if (addr >= 0x2000 && addr < 0x4000) // PPU registers and mirrors
	{
		return m_ppu->CpuPeek(addr & 0x0007 + 0x2000);
	}
	if (addr >= 0x4000 && addr < 0x4018) // APU and IO functionality
	{
		if (addr == 0x4016 || addr == 0x4017) // JOY1 nad JOY2
		{
			return m_controller[addr & 0x0001]->Peek();
		}
		if (addr <= 0x4013 || addr == 0x4015 || addr == 0x4017)
		{
			return m_apu->CpuPeek(addr);
		}
		return 0;
	}
	if (addr >= 0x4018 && addr < 0x4020) // APU and IO functionality Test mode
	{
		return 0;
	}
	else // cartridge space
	{
		return m_cartridge->CpuRead(addr).value_or(0);
	}
}

void Bus::Write(const u16 addr, const u8 val)
{
	if (addr < 0x2000) // Ram and ram mirrors
	{
		m_cpu_ram[addr & 0x07FF] = val;
	}
	else if (addr >= 0x2000 && addr < 0x4000) // PPU registers and mirrors
	{
		m_ppu->CpuWrite(addr & 0x0007 + 0x2000, val);
	}
	else if (addr == 0x4014) // DMA
	{
		m_ppu->CpuWrite(addr, val);
	}
	else if (addr >= 0x4000 && addr < 0x4018) // APU and IO functionality
	{
		if (addr == 0x4016 || addr == 0x4017) // JOY1 nad JOY2
		{
			m_controller[addr & 0x0001]->Write();
		}
		if (addr <= 0x4013 || addr == 0x4015 || addr == 0x4017)
		{
			m_apu->CpuWrite(addr, val);
		}
	}
	else if (addr >= 0x4018 && addr < 0x4020) // APU and IO functionality Test mode
	{
	}
	else // cartridge space
	{
		m_cartridge->CpuWrite(addr, val);
	}
}

void Bus::DMA(u64 cycles)
{
	if (m_ppu->IsDMADummy())
	{
		if (cycles % 2 == 1)
		{
			m_ppu->SetDMADummy(false);
		}
	}
	else
	{
		if (cycles % 2 == 0)
		{
			const u8 dma_data = Read(m_ppu->GetDMAAddr());
			m_ppu->SetDMAData(dma_data);
		}
		else
		{
			m_ppu->DMA();
		}
	}
}

void Bus::Reset()
{
	m_cpu_ram.fill(0);
}

void Bus::Serialize(std::fstream& fs)
{
	Fman::SerializeArrayStoresStatic<u8>(m_cpu_ram);
}

void Bus::Deserialize(std::fstream& fs)
{
	Fman::DeserializeArrayStoresStatic<u8>(m_cpu_ram);
}

}
