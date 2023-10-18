
--[[
	Author: Luis Vijande
	Date: 18/10/2023

	Supposedly this should work with all addressings
	in hex using $numbers
	in bin using %numbers
	in decimal using numbers

	All 54 official instructions should work
	It evaluates line by line
	If you write a number by itself it will add it to the assembly as the number 
]]--

Instructions = {
	ADC = {
		inx = 0x61,
		zpi = 0x65,
		imm = 0x69,
		abs = 0x6d,
		iny = 0x71,
		zpx = 0x75,
		aby = 0x79,
		abx = 0x7d,
	},
	AND = {
		inx = 0x21,
		zpi = 0x25,
		imm = 0x29,
		abs = 0x2d,
		iny = 0x31,
		zpx = 0x35,
		aby = 0x39,
		abx = 0x3d,
	},
	ASL = {
		zpi = 0x06,
		acc = 0x0a,
		abs = 0x0e,
		zpx = 0x16,
		abx = 0x1e,
	},
	BCC = {
		rel = 0x90,
	},
	BCS = {
		rel = 0xb0,
	}, 
	BEQ = {
		rel = 0xf0,
	},
	BIT = {
		zpi = 0x24,
		abs = 0x2c,
	},
	BMI = {
		rel = 0x30,
	},
	BNE = {
		rel = 0xd0,
	},
	BPL = {
		rel = 0x10,
	},
	BRK = {
		imp = 0x00,
	},
	BVC = {
		rel = 0x50,
	},
	BVS = {
		rel = 0x70,
	},
	CLC = {
		imp = 0x18,
	},
	CLD = {
		imp = 0xd8,
	},
	CLI = {
		imp = 0x58,
	},
	CLV = {
		imp = 0xb8
	},
	CMP = {
		inx = 0xc1,
		zpi = 0xc5,
		imm = 0xc9,
		abs = 0xcd,
		iny = 0xd1,
		zpx = 0xd5,
		aby = 0xd9,
		abx = 0xdd,
	},
	CPX = {
		imm = 0xe0,
		zpi = 0xe4,
		abs = 0xec,
	},
	CPY = {
		imm = 0xc0,
		zpi = 0xc4,
		abs = 0xcc,
	},
	DEC = {
		zpi = 0xc6,
		zpx = 0xd6,
		abs = 0xce,
		abx = 0xde,
	},
	DEX = {
		imp = 0xca,
	},
	DEY = {
		imp = 0x88,
	},
	EOR = {
		inx = 0x41,
		zpi = 0x45,
		imm = 0x49,
		abs = 0x4d,
		iny = 0x51,
		zpx = 0x55,
		aby = 0x59,
		abx = 0x5d,
	},
	INC = {
		zpi = 0xe6,
		zpx = 0xf6,
		abs = 0xee,
		abx = 0xfe,
	},
	INX = {
		imp = 0xe8,
	},
	INY = {
		imp = 0xc8,
	},
	JMP = {
		abs = 0x4c,
		ind = 0x6c,
	},
	JSR = {
		abs = 0x20,
	},
	LDA = {
		inx = 0xa1,
		zpi = 0xa5,
		imm = 0xa9,
		abs = 0xad,
		iny = 0xb1,
		zpx = 0xb5,
		aby = 0xb9,
		abx = 0xbd,
	},
	LDX = {
		imm = 0xa2,
		zpi = 0xa6,
		zpy = 0xb6,
		abs = 0xae,
		aby = 0xbe,
	},
	LDY = {
		imm = 0xa0,
		zpi = 0xa4,
		zpx = 0xb4,
		abs = 0xac,
		abx = 0xbc,
	},
	LSR = {
		acc = 0x4a,
		zpi = 0x46,
		zpx = 0x56,
		abs = 0x4e,
		abx = 0x5e,
	},
	NOP = {
		imp = 0xea,
	},
	ORA = {
		inx = 0x01,
		zpi = 0x05,
		imm = 0x09,
		abs = 0x0d,
		iny = 0x11,
		zpx = 0x15,
		aby = 0x19,
		abx = 0x1d,
	},
	PHA = {
		imp = 0x48,
	},
	PHP = {
		imp = 0x08,
	},
	PLA = {
		imp = 0x68,
	},
	PLP = {
		imp = 0x28,
	},
	ROL = {
		acc = 0x2a,
		zpi = 0x26,
		zpx = 0x36,
		abs = 0x2e,
		abx = 0x3e,
	},
	ROR = {
		acc = 0x6a,
		zpi = 0x66,
		zpx = 0x76,
		abs = 0x6e,
		abx = 0x7e,
	},
	RTI = {
		imp = 0x40,
	},
	RTS = {
		imp = 0x60,
	},
	SBC = {
		inx = 0xe1,
		zpi = 0xe5,
		imm = 0xe9,
		abs = 0xed,
		iny = 0xf1,
		zpx = 0xf5,
		aby = 0xf9,
		abx = 0xfd,
	},
	SEC = {
		imp = 0x38,
	},
	SED = {
		imp = 0xf8,
	},
	SEI = {
		imp = 0x78,
	},
	STA = {
		inx = 0x81,
		zpi = 0x85,
		abs = 0x8d,
		iny = 0x91,
		zpx = 0x95,
		aby = 0x99,
		abx = 0x9d,
	},
	STX = {
		zpi = 0x86,
		zpy = 0x96,
		abs = 0x8e,
	},
	STY = {
		zpi = 0x84,
		zpx = 0x94,
		abs = 0x8c,
	},
	TAX = {
		imp = 0xaa,
	},
	TAY = {
		imp = 0xa8,
	},
	TSX = {
		imp = 0xba,
	},
	TXA = {
		imp = 0x8a,
	},
	TXS = {
		imp = 0x9a,
	},
	TYA = {
		imp = 0x98,
	},
}

function GetAddressing(text)
	local res ={}

	if text == nil then
		res['addr'] = 'abs'

	elseif text:match '^%*[-+]%d?%d$' then
		res['addr'] = 'rel'
		res['out'] = tonumber(text:match '([+-]%d+)')

	elseif text:match '^$%x+$' then
		local num = tonumber(text:match '(%x+)', 16)
		if num < 256 then
			res['addr'] = 'zpi'
			res['out'] = num
		else
			res['addr'] = 'abs'
			res['out'] = {}
			res['out'][1] = num & 0x00ff
			res['out'][2] = (num & 0xff00) >> 8
		end
	elseif text:match '^%d+$' then
		local num = tonumber(text:match '(%d+)')
		if num < 256 then
			res['addr'] = 'zpi'
			res['out'] = num
		else
			res['addr'] = 'abs'
			res['out'] = {}
			res['out'][1] = num & 0x00ff
			res['out'][2] = (num & 0xff00) >> 8
		end
	elseif text:match '^%%[01]+$' then
		local num = tonumber(text:match '([01]+)', 2)
		if num < 256 then
			res['addr'] = 'zpi'
			res['out'] = num
		else
			res['addr'] = 'abs'
			res['out'] = {}
			res['out'][1] = num & 0x00ff
			res['out'][2] = (num & 0xff00) >> 8
		end

	elseif text:match '^$%x+,%s*,%s*x$' then
		local num = tonumber(text:match '(%x+)', 16)
		if num < 256 then
			res['addr'] = 'zpx'
			res['out'] = num
		else
			res['addr'] = 'abx'
			res['out'] = {}
			res['out'][1] = num & 0x00ff
			res['out'][2] = (num & 0xff00) >> 8
		end
	elseif text:match '^%d+%s*,%s*x$' then
		local num = tonumber(text:match '(%d+)')
		if num < 256 then
			res['addr'] = 'zpx'
			res['out'] = num
		else
			res['addr'] = 'abx'
			res['out'] = {}
			res['out'][1] = num & 0x00ff
			res['out'][2] = (num & 0xff00) >> 8
		end
	elseif text:match '^%%[01]+%s*,%s*x$' then
		local num = tonumber(text:match '([01]+)', 2)
		if num < 256 then
			res['addr'] = 'zpx'
			res['out'] = num
		else
			res['addr'] = 'abx'
			res['out'] = {}
			res['out'][1] = num & 0x00ff
			res['out'][2] = (num & 0xff00) >> 8
		end

	elseif text:match '^$%x+,%s*,%s*y$' then
		local num = tonumber(text:match '(%x+)', 16)
		if num < 256 then
			res['addr'] = 'zpy'
			res['out'] = num
		else
			res['addr'] = 'aby'
			res['out'] = {}
			res['out'][1] = num & 0x00ff
			res['out'][2] = (num & 0xff00) >> 8
		end
	elseif text:match '^%d+%s*,%s*y$' then
		local num = tonumber(text:match '(%d+)')
		if num < 256 then
			res['addr'] = 'zpy'
			res['out'] = num
		else
			res['addr'] = 'aby'
			res['out'] = {}
			res['out'][1] = num & 0x00ff
			res['out'][2] = (num & 0xff00) >> 8
		end
	elseif text:match '^%%[01]+%s*,%s*y$' then
		local num = tonumber(text:match '([01]+)', 2)
		if num < 256 then
			res['addr'] = 'zpy'
			res['out'] = num
		else
			res['addr'] = 'aby'
			res['out'] = {}
			res['out'][1] = num & 0x00ff
			res['out'][2] = (num & 0xff00) >> 8
		end

	elseif text:match '^%(%s*$%x+%s*,%s*x%s*%)$' then
		res['addr'] = 'inx'
		res['out'] = tonumber(text:match '(%x+)', 16)
	elseif text:match '^%(%s*%d+%s*,%s*x%s*%)$' then
		res['addr'] = 'inx'
		res['out'] = tonumber(text:match '(%d+)')
	elseif text:match '^%(%s*%%[01]+%s*,%s*x%s*%)$' then
		res['addr'] = 'inx'
		res['out'] = tonumber(text:match '([01]+)', 2)

	elseif text:match '^%(%s*$%x+%s*%)%s*,%s*y$' then
		res['addr'] = 'iny'
		res['out'] = tonumber(text:match '(%x+)', 16)
	elseif text:match '^%(%s*%d+%s*%)%s*,%s*y$' then
		res['addr'] = 'iny'
		res['out'] = tonumber(text:match '(%d+)')
	elseif text:match '^%(%s*%%[01]%s*%)%s*,%s*y$' then
		res['addr'] = 'iny'
		res['out'] = tonumber(text:match '([01]+)', 2)

	elseif text:match '^%(%s*$%x+%s*%)$' then
		local num = tonumber(text:match '(%x+)', 16)
		res['addr'] = 'ind'
		res['out'] = {}
		res['out'][1] = num & 0x00ff
		res['out'][2] = (num & 0xff00) >> 8
	elseif text:match '^%(%s*%d+%s*%)$' then
		local num = tonumber(text:match '(%d+)')
		res['addr'] = 'ind'
		res['out'] = {}
		res['out'][1] = num & 0x00ff
		res['out'][2] = (num & 0xff00) >> 8
	elseif text:match '^%(%s*%%[01]+%s*%)$' then
		local num = tonumber(text:match '([01]+)', 2)
		res['addr'] = 'ind'
		res['out'] = {}
		res['out'][1] = num & 0x00ff
		res['out'][2] = (num & 0xff00) >> 8


	elseif text:match '^#$%x+$' then
		res['addr'] = 'imm'
		res['out'] = tonumber(text:match '(%x+)', 16)
	elseif text:match '^#%d+$' then
		res['addr'] = 'imm'
		res['out'] = tonumber(text:match '(%d+)')
	elseif text:match '^#%%[01]+$' then
		res['addr'] = 'imm'
		res['out'] = tonumber(text:match '([01]+)', 2)

	elseif text:match '^[Aa]$' then
		res['addr'] = 'acc'
	end

	return res
end

function AssembleLine(line)
	if line == '' or line == nil then
		return nil
	end
	local assembly = {}
	if line:find '%a%a%a' then

		local instr = string.upper(line:match '^%s*(%a%a%a)')
		local addr  = line:match '^%s*%a%a%a%s+(.+)%s*$'
		local addrType = GetAddressing(addr)
		local an = addrType['addr']
		local out = addrType['out']
		-- check if instruction
		if instr:match('%a%a%a') then
			if Instructions[instr] ~= nil and Instructions[instr][an] ~= nil then
				table.insert(assembly, Instructions[instr][an])
				-- do nothing
				if an == 'imp' or an == 'acc' then
				-- 8-bit instructions
				elseif an == 'imm' or an == 'zpi' or an == 'zpx' or an == 'zpy' or an == 'rel' or an == 'iny' or an == 'inx' then
					table.insert(assembly, out)
				-- 16-bit instructions
				elseif (an == 'abs' or an == 'abx' or an == 'aby' or an == 'ind' ) and type(out) == 'table' then
					table.insert(assembly, out[1])
					table.insert(assembly, out[2])
				end
			else
				error('Invalid operation')
			end
		end
	else
		if line:match '^%s*$%x+$%s*$' then
			table.insert(assembly, tonumber(line:match '(%x+)', 16))
		elseif line:match '^%s*%d+%s*$' then
			table.insert(assembly, tonumber(line:match '(%d+)'))
		elseif line:match '^%s*%%[01]+%s*$' then
			table.insert(assembly, tonumber(line:match '([01]+)', 2))
		end
	end
	return assembly
end

function Assemble(code)
	local assembly = {}
	for line in code:gmatch '([^\n]+)' do
		local res = AssembleLine(line)
		if res ~= nil then
			for _, i in ipairs(res) do
				table.insert(assembly, i)
			end
		end
	end
	return assembly
end
