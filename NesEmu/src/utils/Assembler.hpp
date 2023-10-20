#pragma once

#include <string_view>
#include <unordered_map>
#include <ranges>
#include <cstdint>
#include <ctre/ctre.hpp>

#if defined(DEBUG)
	#define ASSE_LOG_INFO(x) std::cout << "[Assembler INFO] " << __FILE__ << "," << __LINE__ << " " << x
	#define ASSE_LOG_ERROR(x) std::cerr << "[Assembler ERROR] " << __FILE__ << "," << __LINE__ << " " << x
#else 
	#define ASSE_LOG_INFO(x) std::cout << "[Assembler INFO] " << x
	#define ASSE_LOG_ERROR(x) std::cerr << "[Assembler ERROR] " << x
#endif


#define DIRECT_ACCESS "DIRECT_ACCESS"

using InstrData = std::unordered_map<std::string_view,std::unordered_map<std::string_view,uint8_t>>;

namespace A6502
{

struct AddressingMode
{
	std::optional<std::string> name;
	std::optional<uint8_t> lo;
	std::optional<uint8_t> hi;
};


// usage of c++20
template<class T>
concept MemoryAccessor = requires(T bus)
{
	{bus.Read(std::declval<uint16_t>())} -> std::same_as<uint8_t>;
	{bus.Write(std::declval<uint16_t>(),std::declval<uint8_t>())} -> std::same_as<void>;
};

template<MemoryAccessor Bus>
class Assembler
{
public:
	
	void Link(Bus* bus)
	{
		m_bus = bus;
	}

	void Clean()
	{
		m_assembly.clear();
		m_labelPos.clear();
		m_labels.clear();
		m_bus = nullptr;
	}

	void Assemble(std::string_view code)
	{
		auto lines = splittrim(std::string(code), "\n");
		for (const auto line : lines)
		{
			bool goahead = false;
			// split into instruction and addressing
			auto parts = splittrim(line, " ");
			if (parts.size() == 1)
			{
				auto val = parts[0];
				auto noprefix = val.substr(1);
				if (val[0] == '$')
				{
					m_assembly.push_back(std::stoi(noprefix, nullptr, 16));
				}
				// single value bin
				else if (val[0] == '%')
				{
					m_assembly.push_back(std::stoi(noprefix, nullptr, 2));
				}
				// labels
				else if (val[0] == ':')
				{
					m_labelPos[noprefix] = static_cast<uint8_t>(m_assembly.size());
				}
				// single value dec
				else if (std::isdigit(val[0]) || (val[0] == '-' && std::isdigit(val[1])) )
				{
					m_assembly.push_back(std::stoi(val, nullptr, 10));
				}
				else
				{
					goahead = true;
				}
			}
			if (parts[0][0] != ';' && (goahead || parts.size() > 1))
			{
				AddressingMode addressing;
				std::string instr = upper(parts[0]);
				std::string addr = "";
				for (int i = 1; i < parts.size(); ++i)
				{
					addr += parts[i];
				}
				// direct memory access
				std::optional<AddressingMode> dAccess;
				if (instr[0] == '&') 
				{
					dAccess = getAddressing(instr);
				}
				addressing = getAddressing(addr);
				
				auto [name, lo, hi] = addressing;
				if (dAccess && name)
				{
					m_bus->Write(*(*dAccess).hi << 8 | *(*dAccess).lo, *lo);
					continue;
				}
				else if (dAccess && !name)
				{
					ASSE_LOG_ERROR("No value provided for direct memory addresing\n");
				}

				// c++20
				if (!s_instrData.contains(instr))
				{
					ASSE_LOG_ERROR("Instruction " << instr << " does not exists\n");
					ASSE_LOG_INFO("Fatal error, can not continue\n");

					return;
				}
				auto& instrData = s_instrData[instr];
				if (!name)
				{
					ASSE_LOG_INFO("Fatal error, can not continue\n");
					return;
				}
				if (!instrData.contains(*name))
				{
					ASSE_LOG_ERROR(*name << " is not a valid addressing mode for " << instr << "\n");
					ASSE_LOG_INFO("Fatal error, can not continue\n");

					return;
				}
				m_assembly.push_back(instrData[*name]);
				// 3 bytes
				if (hi && lo)
				{
					m_assembly.push_back(*lo);
					m_assembly.push_back(*hi);
				}
				// 2 byte
				else if (lo && !hi)
				{
					m_assembly.push_back(*lo);
				}
				// do nothing for 1 byte
			}
			// comments
			else
			{
				continue;
			}
		}
		for (const auto line : lines)
		{
			if (line[0] != ':')
			{
				continue;
			}
			std::string lbl = line.substr(1);
			if (m_labelPos.contains(lbl))
			{
				uint8_t lblPos = m_labelPos[lbl];
				auto& vec = m_labels[lbl];
				for (const auto pos : vec)
				{
					m_assembly[pos] = lblPos;
				}
			}
			else
			{
				ASSE_LOG_ERROR("label [" << lbl << "] not found\n");
				ASSE_LOG_INFO("Fatal error, can not continue\n");

				return;
			}
		}

		// load assembly in MemoryAccessor
		for (size_t i = 0; i < m_assembly.size(); ++i)
		{
			m_bus->Write(static_cast<uint16_t>(i), m_assembly[i]);
		}
	}

private:

	constexpr std::string upper(const std::string& sv) noexcept
	{
		auto _upper = [](unsigned char c) -> unsigned char { return std::toupper(c); };
		std::string s(sv);
		std::transform(s.begin(), s.end(), s.begin(), _upper);
		return s;
	}

	constexpr AddressingMode getAddressing(const std::string& text) noexcept
	{
		AddressingMode a;
		std::optional<uint16_t> temp = std::nullopt;
		int nbytes = 0;
		std::string_view encoding = "";
		std::string number;
		bool error = false;
		// imp
		if (text.empty())
		{
			a.name = "imp";
		}
		// acc
		else if (ctre::match<"^[aA]$">(text))
		{
			a.name = "acc";
			
		}
		// direct memory access
		else if (auto [whole, mode, num] = ctre::match<"^&([$%]?)(.+)$">(text); whole)
		{
			a.name = DIRECT_ACCESS;
			encoding = mode;
			number = num;
			nbytes = 2;
		}
		// 16 bit explicit hex for abs abx aby
		else if (auto [whole, num, reg, type] = ctre::match<"^\\$([A-Fa-f0-9]{3,4})(,([xy]))?$">(text); whole)
		{
			temp = std::stoi(num.str(), nullptr, 16);

			if (reg)
			{
				a.name = "ab";
				*a.name += type;
			}
			else
			{
				a.name = "abs";
			}

			nbytes = 2;
			encoding = "$";
			number = num;
		}
		// either abx, zpx, aby or zpy
		else if (auto [whole, mode, num, type] = ctre::match<"^([$%]?)([^(]+),([xy])$">(text); whole)
		{
			encoding = mode;
			number = num;
			if (temp > 255)
			{
				a.name = "ab";
				*a.name += type;
				nbytes = 2;
			}
			else
			{
				a.name = "zp";
				*a.name += type;

				nbytes = 1;
			}
		}
		// inx
		else if (auto [whole, mode, num] = ctre::match<"^\\(([$%]?)(.+),x\\)$">(text); whole)
		{
			encoding = mode;
			number = num;
			a.name = "inx";
			nbytes = 1;
		}
		// iny
		else if (auto [whole, mode, num] = ctre::match<"^\\(([$%]?)(.+)\\),y$">(text); whole)
		{
			encoding = mode;
			number = num;
			a.name = "iny";
			nbytes = 1;
		}
		// ind
		else if (auto [whole, mode, num] = ctre::match<"^\\(([$%]?)(.+)\\)$">(text); whole)
		{
			encoding = mode;
			number = num;
			a.name = "ind";
			nbytes = 2;
		}
		// imm
		else if (auto [whole, mode, num] = ctre::match<"^#([$%]?)(.+)$">(text); whole)
		{
			encoding = mode;
			number = num;
			a.name = "imm";
			nbytes = 1;
		}
		// labels
		else if (auto [whole, label] = ctre::match<"^([a-zA-Z].+)$">(text); whole)
		{
			m_labels[label.str()].push_back(static_cast<uint8_t>(m_assembly.size() + 1));
			a.name = "rel";
			a.lo = 0;
		}
		// rel if negative special case
		else if (auto [whole, num] = ctre::match<"^(-\\d+)$">(text); whole)
		{
			a.name = "rel";
			number = num;
			encoding = "";
			nbytes = 1;
		}
		// abs or zpi
		else if (auto [whole, mode, num] = ctre::match<"^([$%]?)(.+)$">(text); whole)
		{
			encoding = mode;
			number = num;
			if (temp > 255)
			{
				a.name = "abs";
				nbytes = 2;
			}
			else
			{
				a.name = "zpi";
				nbytes = 1;

			}
		}
		else
		{
			ASSE_LOG_ERROR("Unrecognized addressing mode\n");
			error = true;
		}

		if (!error && nbytes > 0)
		{
			temp = decode(number, encoding);
			if (!temp)
			{
				error = true;
			}
			else 
			{
				a.lo = *temp & 0x00ff;
			}
		}
		if (!error && nbytes == 2)
		{
			a.hi = (*temp & 0xff00) >> 8;
		}

		if (error)
		{
			a.name = std::nullopt;
			a.lo = std::nullopt;
			a.hi = std::nullopt;
		}
		return a;
	}

	constexpr std::vector<std::string> splittrim(std::string s, std::string_view delim) noexcept
	{
		std::vector<std::string> res;
		auto nospace = [](unsigned char c) -> unsigned char { return !std::isspace(c); };

		size_t pos;
		while ((pos = s.find(delim)) != std::string::npos)
		{
			std::string token = s.substr(0, pos);
			token.erase(
				std::ranges::find_if(token | std::views::reverse, nospace).base(),
				token.end()
			);
			token.erase(
				token.begin(),
				std::ranges::find_if(token, nospace)
			);
			s.erase(0, pos + delim.length());

			if (token.empty()) continue;
			res.push_back(token);
		}
		s.erase(
			std::ranges::find_if(s | std::views::reverse, nospace).base(),
			s.end()
		);
		s.erase(
			s.begin(),
			std::ranges::find_if(s, nospace)
		);
		s.erase(0, pos + delim.length());

		if (s.empty()) return res;
		res.push_back(s);
		return res;
	}

	constexpr std::optional<uint16_t> decode(const std::string& num, std::string_view encoding)
	{	
		if (encoding == "$" && ishexadecimal(num))
		{
			return std::stoi(num, nullptr, 16);
		}
		else if (encoding == "%" && isbinary(num))
		{
			return std::stoi(num, nullptr, 2);
		}
		else if (isdecimal(num))
		{
			return std::stoi(num, nullptr, 10);
		}
		else
		{
			ASSE_LOG_ERROR(num << " is not a number\n");
			return std::nullopt;
		}

	}

	constexpr bool isdecimal(std::string_view s) noexcept
	{
		auto isinvalid = [](unsigned char c)
		{
			return !(
				std::isdigit(c) ||
				c == '-'
			);
		};
		return std::ranges::count_if(s, isinvalid) == 0;
	}

	constexpr bool ishexadecimal(std::string_view s) noexcept
	{
		auto isinvalid = [](unsigned char c)
		{
			c = std::toupper(c);
			return !(
				std::isdigit(c) || 
				c == 'A' ||
				c == 'B' ||
				c == 'C' ||
				c == 'D' ||
				c == 'E' ||
				c == 'F'
			);
		};
		return std::ranges::count_if(s, isinvalid) == 0;
	}

	constexpr bool isbinary(std::string_view s) noexcept
	{
		auto isinvalid = [](unsigned char c)
		{
			return !(
				c == '0' ||
				c == '1'
			);
		};
		return std::ranges::count_if(s, isinvalid) == 0;
	}

private:

	std::vector<uint8_t> m_assembly;
	std::unordered_map<std::string, std::vector<uint8_t>> m_labels;
	std::unordered_map<std::string, uint8_t> m_labelPos;
	
	Bus* m_bus = nullptr;
	// C++17 my saviour
	static inline InstrData s_instrData = {
		{"ADC",{{"imm",0x69},{"zpi",0x65},{"zpx",0x75},{"abs",0x6d},{"abx",0x7d},{"aby",0x79},{"inx",0x61},{"iny",0x71}}},
		{"AND",{{"imm",0x29},{"zpi",0x25},{"zpx",0x35},{"abs",0x2d},{"abx",0x3d},{"aby",0x39},{"inx",0x21},{"iny",0x31}}},
		{"ASL",{{"acc",0x0a},{"zpi",0x06},{"zpx",0x16},{"abs",0x0e},{"abx",0x1e}}},
		{"BCC",{{"rel",0x90},{"zpi",0x90}}},
		{"BCS",{{"rel",0xb0},{"zpi",0xb0}}},
		{"BEQ",{{"rel",0xf0},{"zpi",0xf0}}},
		{"BIT",{{"zpi",0x24},{"abs",0x2c}}},
		{"BMI",{{"rel",0x30},{"zpi",0x30}}},
		{"BNE",{{"rel",0xd0},{"zpi",0xd0}}},
		{"BPL",{{"rel",0x10},{"zpi",0x10}}},
		{"BRK",{{"imp",0x00}}},
		{"BVC",{{"rel",0x50},{"zpi",0x50}}},
		{"BVS",{{"rel",0x70},{"zpi",0x70}}},
		{"CLC",{{"imp",0x18},{"zpi",0x18}}},
		{"CLD",{{"imp",0xd8}}},
		{"CLI",{{"imp",0x58}}},
		{"CLV",{{"imp",0xb8}}},
		{"CMP",{{"imm",0xc9},{"zpi",0xc5},{"zpx",0xd5},{"abs",0xcd},{"abx",0xdd},{"aby",0xd9},{"inx",0xc1},{"iny",0xd1}}},
		{"CPX",{{"imm",0xe0},{"zpi",0xe4},{"abs",0xec}}},
		{"CPY",{{"imm",0xc0},{"zpi",0xc4},{"abs",0xcc}}},
		{"DEC",{{"zpi",0xc6},{"zpx",0xd6},{"abs",0xce},{"abx",0xde}}},
		{"DEX",{{"imp",0xca}}},
		{"DEY",{{"imp",0x88}}},
		{"EOR",{{"imm",0x49},{"zpi",0x45},{"zpx",0x55},{"abs",0x4d},{"abx",0x5d},{"aby",0x59},{"inx",0x41},{"iny",0x51}}},
		{"INC",{{"zpi",0xe6},{"zpx",0xf6},{"abs",0xee},{"abx",0xfe}}},
		{"INX",{{"imp",0xe8}}},
		{"INY",{{"imp",0xc8}}},
		{"JMP",{{"abs",0x4c},{"ind",0x6c}}},
		{"JSR",{{"abs",0x20}}},
		{"LDA",{{"imm",0xa9},{"zpi",0xa5},{"zpx",0xb5},{"abs",0xad},{"abx",0xbd},{"aby",0xb9},{"inx",0xa1},{"iny",0xb1}}},
		{"LDX",{{"imm",0xa2},{"zpi",0xa6},{"zpy",0xb6},{"abs",0xae},{"aby",0xbe}}},
		{"LDY",{{"imm",0xa0},{"zpi",0xa4},{"zpx",0xb4},{"abs",0xac},{"abx",0xbc}}},
		{"LSR",{{"acc",0x4a},{"zpi",0x46},{"zpx",0x56},{"abs",0x4e},{"abx",0x5e}}},
		{"NOP",{{"imp",0xea}}},
		{"ORA",{{"imm",0x09},{"zpi",0x05},{"zpx",0x15},{"abs",0x0d},{"abx",0x1d},{"aby",0x19},{"inx",0x01},{"iny",0x11}}},
		{"PHA",{{"imp",0x48}}},
		{"PHP",{{"imp",0x08}}},
		{"PLA",{{"imp",0x68}}},
		{"PLP",{{"imp",0x28}}},
		{"ROL",{{"acc",0x2a},{"zpi",0x26},{"zpx",0x36},{"abs",0x2e},{"abx",0x3e}}},
		{"ROR",{{"acc",0x6a},{"zpi",0x66},{"zpx",0x76},{"abs",0x6e},{"abx",0x7e}}},
		{"RTI",{{"imp",0x40}}},
		{"RTS",{{"imp",0x60}}},
		{"SBC",{{"imm",0xe9},{"zpi",0xe5},{"zpx",0xf5},{"abs",0xed},{"abx",0xfd},{"aby",0xf9},{"inx",0xe1},{"iny",0xf1}}},
		{"SEC",{{"imp",0x38}}},
		{"SED",{{"imp",0xf8}}},
		{"SEI",{{"imp",0x78}}},
		{"STA",{{"inx",0x81},{"zpi",0x85},{"abs",0x8d},{"iny",0x91},{"zpx",0x95},{"aby",0x99},{"abx",0x9d}}},
		{"STX",{{"zpi",0x86},{"zpy",0x96},{"abs",0x8e}}},
		{"STY",{{"zpi",0x84},{"zpx",0x94},{"abs",0x8c}}},
		{"TAX",{{"imp",0xaa}}},
		{"TAY",{{"imp",0xa8}}},
		{"TSX",{{"imp",0xba}}},
		{"TXA",{{"imp",0x8a}}},
		{"TXS",{{"imp",0x9a}}},
		{"TYA",{{"imp",0x98}}}
	};

};
}