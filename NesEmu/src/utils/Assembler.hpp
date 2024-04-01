#ifndef A6502_TOY_ASSEMBLER
#define A6502_TOY_ASSEMBLER

#include <string_view>
#include <optional>
#include <unordered_map>
#include <ranges>
#include <cstdint>

#include <ctre/ctre.hpp>

#if defined(DEBUG)
#define ASSE_LOG_INFO(x) std::cout << "[Assembler INFO] " << x
#define ASSE_LOG_ERROR(x) std::cerr << "[Assembler ERROR] " << x
#else
#define ASSE_LOG_INFO(x) 
#define ASSE_LOG_ERROR(x) 
#endif

namespace A6502
{

struct AddressingMode
{
	std::string name;
	std::optional<uint8_t> lo;
	std::optional<uint8_t> hi;
};
template <class T>
concept MemoryAccessor = requires(T bus) {
	{
		bus.Read(std::declval<uint16_t>())
	} -> std::same_as<uint8_t>;
	{
		bus.Write(std::declval<uint16_t>(), std::declval<uint8_t>())
	} -> std::same_as<void>;
};

template <MemoryAccessor Bus>
class Assembler
{
public:
	void Link(Bus *bus)
	{
		m_bus = bus;
	}

	void Clean()
	{
		m_assembly.clear();
		m_directAccess.clear();
		m_bus = nullptr;
	}

	void Assemble(const std::string &code)
	{
		const auto lines = splittrim(code, "\n");
		for (const auto &line : lines)
		{
			// split into instruction and addressing
			const auto parts = splittrim(upper(line), " ");

			_assemble(parts);
		}
		store();
	}

private:
	void store()
	{
		for (size_t i = 0; i < m_assembly.size(); ++i)
		{
			m_bus->Write(static_cast<uint16_t>(i), m_assembly[i]);
		}

		for (auto [addr, val] : m_directAccess)
		{
			m_bus->Write(addr, val);
		}
	}
	void _assemble(const std::vector<std::string> &parts)
	{
		const std::string instr = parts[0];
		// can't += string view
		std::string addr = "";
		for (int i = 1; i < parts.size(); ++i)
		{
			addr += parts[i];
		}
		const auto [name, lo, hi] = parse(addr);
		if (name.empty())
		{
			ASSE_LOG_INFO("Fatal error, can not continue\n");
			return;
		}
		// direct memory access
		if (instr[0] == '&')
		{
			auto [_, dlo, dhi] = parse(instr);
			m_directAccess[*dhi << 8 | *dlo] = *lo;
			return;
		}

		// c++20
		if (!s_instrData.contains(instr))
		{
			ASSE_LOG_ERROR("Instruction " << instr << " does not exists\n");
			ASSE_LOG_INFO("Fatal error, can not continue\n");
			return;
		}
		if (!s_instrData.at(instr).contains(name))
		{
			ASSE_LOG_ERROR(name << " is not a valid addressing mode for " << instr << "\n");
			ASSE_LOG_INFO("Fatal error, can not continue\n");
			return;
		}
		m_assembly.push_back(s_instrData.at(instr).at(name));
		if (instr == "BRK")
		{
			// push nop
			m_assembly.push_back(0xEA);
		}
		// 3 bytes
		if (hi && lo)
		{
			m_assembly.push_back(*lo);
			m_assembly.push_back(*hi);
		}
		// 2 byte
		else if (lo)
		{
			m_assembly.push_back(*lo);
		}
		// do nothing for 1 byte
	}

	std::string upper(std::string s) const noexcept
	{
		const auto isupper = [](unsigned char c) -> unsigned char
		{ return std::toupper(c); };
		std::transform(s.begin(), s.end(), s.begin(), isupper);
		return s;
	}

	AddressingMode parse(const std::string &text)
	{
		AddressingMode a;
		std::optional<uint16_t> temp = std::nullopt;
		int nbytes = 0;
		// IMP
		if (text.empty())
		{
			a.name = "IMP";
		}
		// ACC
		else if (text == "a" || text == "A")
		{
			a.name = "ACC";
		}
		// direct memory access
		else if (auto [whole, mode, num] = ctre::match<"^&([$]?)([0-9A-F]+)$">(text); whole)
		{
			a.name = "directAccess";
			temp = decode(num.str(), mode);
			nbytes = 2;
		}
		// 16 bit explicit hex for ABS ABX ABY
		else if (auto [whole, num, reg, type] = ctre::match<"^\\$([A-F0-9]{3,4})(,([XY]))?$">(text); whole)
		{
			if (reg)
			{
				a.name = "AB";
				a.name += type;
			}
			else
			{
				a.name = "ABS";
			}

			nbytes = 2;
			temp = decode(num.str(), "$");
		}
		// either ABX, ZPX, ABY or ZPY
		else if (auto [whole, mode, num, type] = ctre::match<"^([$]?)([A-Z0-9]+),([XY])$">(text); whole)
		{
			temp = decode(num.str(), mode);
			if (temp > 255)
			{
				a.name = "AB";
				a.name += type;
				nbytes = 2;
			}
			else
			{
				a.name = "ZP";
				a.name += type;

				nbytes = 1;
			}
		}
		// INX
		else if (auto [whole, mode, num] = ctre::match<"^\\(([$]?)([A-Z0-9]+),X\\)$">(text); whole)
		{
			temp = decode(num.str(), mode);
			a.name = "INX";
			nbytes = 1;
		}
		// INY
		else if (auto [whole, mode, num] = ctre::match<"^\\(([$]?)([A-Z0-9]+)\\),Y$">(text); whole)
		{
			temp = decode(num.str(), mode);
			a.name = "INY";
			nbytes = 1;
		}
		// IND
		else if (auto [whole, mode, num] = ctre::match<"^\\(([$]?)([A-Z0-9]+)\\)$">(text); whole)
		{
			temp = decode(num.str(), mode);
			a.name = "IND";
			nbytes = 2;
		}
		// IMM
		else if (auto [whole, mode, num] = ctre::match<"^#([$]?)([\\-A-Z0-9]+)$">(text); whole)
		{
			temp = decode(num.str(), mode);
			a.name = "IMM";
			nbytes = 1;
		}
		// REL if negative special case
		else if (auto [whole, num] = ctre::match<"^(-\\d+)$">(text); whole)
		{
			temp = decode(num.str(), "");
			a.name = "REL";
			nbytes = 1;
		}
		// ABS or ZPI
		else if (auto [whole, mode, num] = ctre::match<"^([$]?)([A-Z0-9]+)$">(text); whole)
		{
			temp = decode(num.str(), mode);
			if (temp > 255)
			{
				a.name = "ABS";
				nbytes = 2;
			}
			else
			{
				a.name = "ZPI";
				nbytes = 1;
			}
		}
		else
		{
			ASSE_LOG_ERROR("Unrecognized addressing mode\n");
			return a;
		}

		if (temp && nbytes == 1)
		{
			a.lo = *temp & 0x00ff;
		}
		else if (temp && nbytes == 2)
		{
			a.lo = *temp & 0x00ff;
			a.hi = (*temp & 0xff00) >> 8;
		}
		else if (!temp)
		{
			return a;
		}
		return a;
	}

	std::vector<std::string> splittrim(std::string s, const std::string_view &delim) const noexcept
	{
		std::vector<std::string> res;
		const auto nospace = [](unsigned char c) -> unsigned char
		{ return !std::isspace(c); };

		size_t pos;
		while ((pos = s.find(delim)) != std::string::npos)
		{
			std::string token = s.substr(0, pos);
			token.erase(std::ranges::find_if(token | std::views::reverse, nospace).base(), token.end());
			token.erase(token.begin(), std::ranges::find_if(token, nospace));
			s.erase(0, pos + delim.length());

			if (token.empty())
				continue;
			res.push_back(token);
		}
		s.erase(std::ranges::find_if(s | std::views::reverse, nospace).base(), s.end());
		s.erase(s.begin(), std::ranges::find_if(s, nospace));
		s.erase(0, pos + delim.length());

		if (s.empty())
			return res;
		res.push_back(s);
		return res;
	}

	std::optional<uint16_t> decode(const std::string &num, const std::string_view &encoding) const
	{
		if (encoding == "$" && ishexadecimal(num))
		{
			return std::stoi(num, nullptr, 16);
		}
		if (isdecimal(num))
		{
			return std::stoi(num, nullptr, 10);
		}
		ASSE_LOG_ERROR(num << " is not a number\n");
		return std::nullopt;
	}

	bool isdecimal(const std::string_view &s) const noexcept
	{
		const auto isinvalid = [](unsigned char c) -> unsigned char
		{
			return !(std::isdigit(c) || c == '-');
		};
		return std::ranges::count_if(s, isinvalid) == 0;
	}

	bool ishexadecimal(const std::string_view &s) const noexcept
	{
		const auto isinvalid = [](unsigned char c) -> unsigned char
		{
			return !(std::isdigit(c) || (c >= 'A' && c <= 'F'));
		};
		return std::ranges::count_if(s, isinvalid) == 0;
	}

private:
	std::vector<uint8_t> m_assembly;
	std::unordered_map<uint16_t, uint8_t> m_directAccess;
	Bus *m_bus = nullptr;
	// C++17 my saviour
	static inline std::unordered_map<std::string_view, std::unordered_map<std::string_view, uint8_t>> s_instrData = {
		{"ADC", {{"IMM", 0x69}, {"ZPI", 0x65}, {"ZPX", 0x75}, {"ABS", 0x6d}, {"ABX", 0x7d}, {"ABY", 0x79}, {"INX", 0x61}, {"INY", 0x71}}},
		{"AND", {{"IMM", 0x29}, {"ZPI", 0x25}, {"ZPX", 0x35}, {"ABS", 0x2d}, {"ABX", 0x3d}, {"ABY", 0x39}, {"INX", 0x21}, {"INY", 0x31}}},
		{"ASL", {{"ACC", 0x0a}, {"ZPI", 0x06}, {"ZPX", 0x16}, {"ABS", 0x0e}, {"ABX", 0x1e}}},
		{"BCC", {{"REL", 0x90}, {"ZPI", 0x90}}},
		{"BCS", {{"REL", 0xb0}, {"ZPI", 0xb0}}},
		{"BEQ", {{"REL", 0xf0}, {"ZPI", 0xf0}}},
		{"BIT", {{"ZPI", 0x24}, {"ABS", 0x2c}}},
		{"BMI", {{"REL", 0x30}, {"ZPI", 0x30}}},
		{"BNE", {{"REL", 0xd0}, {"ZPI", 0xd0}}},
		{"BPL", {{"REL", 0x10}, {"ZPI", 0x10}}},
		{"BRK", {{"IMP", 0x00}}},
		{"BVC", {{"REL", 0x50}, {"ZPI", 0x50}}},
		{"BVS", {{"REL", 0x70}, {"ZPI", 0x70}}},
		{"CLC", {{"IMP", 0x18}, {"ZPI", 0x18}}},
		{"CLD", {{"IMP", 0xd8}}},
		{"CLI", {{"IMP", 0x58}}},
		{"CLV", {{"IMP", 0xb8}}},
		{"CMP", {{"IMM", 0xc9}, {"ZPI", 0xc5}, {"ZPX", 0xd5}, {"ABS", 0xcd}, {"ABX", 0xdd}, {"ABY", 0xd9}, {"INX", 0xc1}, {"INY", 0xd1}}},
		{"CPX", {{"IMM", 0xe0}, {"ZPI", 0xe4}, {"ABS", 0xec}}},
		{"CPY", {{"IMM", 0xc0}, {"ZPI", 0xc4}, {"ABS", 0xcc}}},
		{"DEC", {{"ZPI", 0xc6}, {"ZPX", 0xd6}, {"ABS", 0xce}, {"ABX", 0xde}}},
		{"DEX", {{"IMP", 0xca}}},
		{"DEY", {{"IMP", 0x88}}},
		{"EOR", {{"IMM", 0x49}, {"ZPI", 0x45}, {"ZPX", 0x55}, {"ABS", 0x4d}, {"ABX", 0x5d}, {"ABY", 0x59}, {"INX", 0x41}, {"INY", 0x51}}},
		{"INC", {{"ZPI", 0xe6}, {"ZPX", 0xf6}, {"ABS", 0xee}, {"ABX", 0xfe}}},
		{"INX", {{"IMP", 0xe8}}},
		{"INY", {{"IMP", 0xc8}}},
		{"JMP", {{"ABS", 0x4c}, {"IND", 0x6c}}},
		{"JSR", {{"ABS", 0x20}}},
		{"LDA", {{"IMM", 0xa9}, {"ZPI", 0xa5}, {"ZPX", 0xb5}, {"ABS", 0xad}, {"ABX", 0xbd}, {"ABY", 0xb9}, {"INX", 0xa1}, {"INY", 0xb1}}},
		{"LDX", {{"IMM", 0xa2}, {"ZPI", 0xa6}, {"ZPY", 0xb6}, {"ABS", 0xae}, {"ABY", 0xbe}}},
		{"LDY", {{"IMM", 0xa0}, {"ZPI", 0xa4}, {"ZPX", 0xb4}, {"ABS", 0xac}, {"ABX", 0xbc}}},
		{"LSR", {{"ACC", 0x4a}, {"ZPI", 0x46}, {"ZPX", 0x56}, {"ABS", 0x4e}, {"ABX", 0x5e}}},
		{"NOP", {{"IMP", 0xea}}},
		{"ORA", {{"IMM", 0x09}, {"ZPI", 0x05}, {"ZPX", 0x15}, {"ABS", 0x0d}, {"ABX", 0x1d}, {"ABY", 0x19}, {"INX", 0x01}, {"INY", 0x11}}},
		{"PHA", {{"IMP", 0x48}}},
		{"PHP", {{"IMP", 0x08}}},
		{"PLA", {{"IMP", 0x68}}},
		{"PLP", {{"IMP", 0x28}}},
		{"ROL", {{"ACC", 0x2a}, {"ZPI", 0x26}, {"ZPX", 0x36}, {"ABS", 0x2e}, {"ABX", 0x3e}}},
		{"ROR", {{"ACC", 0x6a}, {"ZPI", 0x66}, {"ZPX", 0x76}, {"ABS", 0x6e}, {"ABX", 0x7e}}},
		{"RTI", {{"IMP", 0x40}}},
		{"RTS", {{"IMP", 0x60}}},
		{"SBC", {{"IMM", 0xe9}, {"ZPI", 0xe5}, {"ZPX", 0xf5}, {"ABS", 0xed}, {"ABX", 0xfd}, {"ABY", 0xf9}, {"INX", 0xe1}, {"INY", 0xf1}}},
		{"SEC", {{"IMP", 0x38}}},
		{"SED", {{"IMP", 0xf8}}},
		{"SEI", {{"IMP", 0x78}}},
		{"STA", {{"INX", 0x81}, {"ZPI", 0x85}, {"ABS", 0x8d}, {"INY", 0x91}, {"ZPX", 0x95}, {"ABY", 0x99}, {"ABX", 0x9d}}},
		{"STX", {{"ZPI", 0x86}, {"ZPY", 0x96}, {"ABS", 0x8e}}},
		{"STY", {{"ZPI", 0x84}, {"ZPX", 0x94}, {"ABS", 0x8c}}},
		{"TAX", {{"IMP", 0xaa}}},
		{"TAY", {{"IMP", 0xa8}}},
		{"TSX", {{"IMP", 0xba}}},
		{"TXA", {{"IMP", 0x8a}}},
		{"TXS", {{"IMP", 0x9a}}},
		{"TYA", {{"IMP", 0x98}}}
	};
};
}
#endif