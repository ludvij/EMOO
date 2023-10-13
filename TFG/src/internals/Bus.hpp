#pragma once

#include "Core.hpp"
namespace Emu
{
	
class Bus
{
public:
	Bus();

	Byte Read(Word addr);
	void Write(Word addr, Byte val);
};

}
