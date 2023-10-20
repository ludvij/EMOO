
--[[
	Author: Luis Vijande
	Date: 18/10/2023

	Supposedly this should work with all addressings
	in hex using $numbers
	in bin using %numbers
	in decimal using numbers
	
	also supports use of labels

	All 54 official instructions should work
	It evaluates line by line
	If you write a number by itself it will add it to the assembly as the number 
]]--

Instructions = { 
	ADC = { imm = 0x69, zpi = 0x65, zpx = 0x75, abs = 0x6d, abx = 0x7d, aby = 0x79, inx = 0x61, iny = 0x71, },
	AND = { imm = 0x29, zpi = 0x25, zpx = 0x35, abs = 0x2d, abx = 0x3d, aby = 0x39, inx = 0x21, iny = 0x31, },
	ASL = { acc = 0x0a, zpi = 0x06, zpx = 0x16, abs = 0x0e, abx = 0x1e, },
	BCC = { rel = 0x90, zpi = 0x90, },
	BCS = { rel = 0xb0, zpi = 0xb0, }, 
	BEQ = { rel = 0xf0, zpi = 0xf0, },
	BIT = { zpi = 0x24, abs = 0x2c, },
	BMI = { rel = 0x30, zpi = 0x30, },
	BNE = { rel = 0xd0, zpi = 0xd0, },
	BPL = { rel = 0x10, zpi = 0x10, },
	BRK = { imp = 0x00, },
	BVC = { rel = 0x50, zpi = 0x50, },
	BVS = { rel = 0x70, zpi = 0x70, },
	CLC = { imp = 0x18, zpi = 0x18, },
	CLD = { imp = 0xd8, },
	CLI = { imp = 0x58, },
	CLV = { imp = 0xb8, },
	CMP = { imm = 0xc9, zpi = 0xc5, zpx = 0xd5, abs = 0xcd, abx = 0xdd, aby = 0xd9, inx = 0xc1, iny = 0xd1, },
	CPX = { imm = 0xe0, zpi = 0xe4, abs = 0xec, },
	CPY = { imm = 0xc0, zpi = 0xc4, abs = 0xcc, },
	DEC = { zpi = 0xc6, zpx = 0xd6, abs = 0xce, abx = 0xde, },
	DEX = { imp = 0xca, },
	DEY = { imp = 0x88, },
	EOR = { imm = 0x49, zpi = 0x45, zpx = 0x55, abs = 0x4d, abx = 0x5d, aby = 0x59, inx = 0x41, iny = 0x51, },
	INC = { zpi = 0xe6, zpx = 0xf6, abs = 0xee, abx = 0xfe, },
	INX = { imp = 0xe8, },
	INY = { imp = 0xc8, },
	JMP = { abs = 0x4c, ind = 0x6c, },
	JSR = { abs = 0x20, },
	LDA = { imm = 0xa9, zpi = 0xa5, zpx = 0xb5, abs = 0xad, abx = 0xbd, aby = 0xb9, inx = 0xa1, iny = 0xb1, },
	LDX = { imm = 0xa2, zpi = 0xa6, zpy = 0xb6, abs = 0xae, aby = 0xbe, },
	LDY = { imm = 0xa0, zpi = 0xa4, zpx = 0xb4, abs = 0xac, abx = 0xbc, },
	LSR = { acc = 0x4a, zpi = 0x46, zpx = 0x56, abs = 0x4e, abx = 0x5e, },
	NOP = { imp = 0xea, },
	ORA = { imm = 0x09, zpi = 0x05, zpx = 0x15, abs = 0x0d, abx = 0x1d, aby = 0x19, inx = 0x01, iny = 0x11, },
	PHA = { imp = 0x48, },
	PHP = { imp = 0x08, },
	PLA = { imp = 0x68, },
	PLP = { imp = 0x28, },
	ROL = { acc = 0x2a, zpi = 0x26, zpx = 0x36, abs = 0x2e, abx = 0x3e, },
	ROR = { acc = 0x6a, zpi = 0x66, zpx = 0x76, abs = 0x6e, abx = 0x7e, },
	RTI = { imp = 0x40, },
	RTS = { imp = 0x60, },
	SBC = { imm = 0xe9, zpi = 0xe5, zpx = 0xf5, abs = 0xed, abx = 0xfd, aby = 0xf9, inx = 0xe1, iny = 0xf1, },
	SEC = { imp = 0x38, },
	SED = { imp = 0xf8, },
	SEI = { imp = 0x78, },
	STA = { inx = 0x81, zpi = 0x85, abs = 0x8d, iny = 0x91, zpx = 0x95, aby = 0x99, abx = 0x9d, },
	STX = { zpi = 0x86, zpy = 0x96, abs = 0x8e, },
	STY = { zpi = 0x84, zpx = 0x94, abs = 0x8c, },
	TAX = { imp = 0xaa, },
	TAY = { imp = 0xa8, },
	TSX = { imp = 0xba, },
	TXA = { imp = 0x8a, },
	TXS = { imp = 0x9a, },
	TYA = { imp = 0x98, },
}

function GetAddressing(text)
	local res = {}
	if text == nil then
		res['addr'] = 'imp'

	elseif text:match '^[Aa]$' then
		res['addr'] = 'acc'
	elseif text:match '^%-%s*%d+$' then
		res['addr'] = 'rel'
		res['lo'] = tonumber(text:match '(%-%d+)')

	elseif text:match '^$%x+$' then
		local num = tonumber(text:match '(%x+)', 16)
		if num < 256 then
			res['addr'] = 'zpi'
			res['lo'] = num
		else
			res['addr'] = 'abs'
			res['lo'] = num & 0x00ff
			res['hi'] = (num & 0xff00) >> 8
		end
	elseif text:match '^%d+$' then
		local num = tonumber(text:match '(%d+)')
		if num < 256 then
			res['addr'] = 'zpi'
			res['lo'] = num
		else
			res['addr'] = 'abs'
			res['lo'] = num & 0x00ff
			res['hi'] = (num & 0xff00) >> 8
		end
	elseif text:match '^%%[01]+$' then
		local num = tonumber(text:match '([01]+)', 2)
		if num < 256 then
			res['addr'] = 'zpi'
			res['lo'] = num
		else
			res['addr'] = 'abs'
			res['lo'] = num & 0x00ff
			res['hi'] = (num & 0xff00) >> 8
		end

	elseif text:match '^$%x+%s*,%s*x$' then
		local num = tonumber(text:match '(%x+)', 16)
		if num < 256 then
			res['addr'] = 'zpx'
			res['lo'] = num
		else
			res['addr'] = 'abx'
			res['lo'] = num & 0x00ff
			res['hi'] = (num & 0xff00) >> 8
		end
	elseif text:match '^%d+%s*,%s*x$' then
		local num = tonumber(text:match '(%d+)')
		if num < 256 then
			res['addr'] = 'zpx'
			res['lo'] = num
		else
			res['addr'] = 'abx'
			res['lo'] = num & 0x00ff
			res['hi'] = (num & 0xff00) >> 8
		end
	elseif text:match '^%%[01]+%s*,%s*x$' then
		local num = tonumber(text:match '([01]+)', 2)
		if num < 256 then
			res['addr'] = 'zpx'
			res['lo'] = num
		else
			res['addr'] = 'abx'
			res['lo'] = num & 0x00ff
			res['hi'] = (num & 0xff00) >> 8
		end

	elseif text:match '^$%x+,%s*,%s*y$' then
		local num = tonumber(text:match '(%x+)', 16)
		if num < 256 then
			res['addr'] = 'zpy'
			res['lo'] = num
		else
			res['addr'] = 'aby'
			res['lo'] = num & 0x00ff
			res['hi'] = (num & 0xff00) >> 8
		end
	elseif text:match '^%d+%s*,%s*y$' then
		local num = tonumber(text:match '(%d+)')
		if num < 256 then
			res['addr'] = 'zpy'
			res['lo'] = num
		else
			res['addr'] = 'aby'
			res['lo'] = num & 0x00ff
			res['hi'] = (num & 0xff00) >> 8
		end
	elseif text:match '^%%[01]+%s*,%s*y$' then
		local num = tonumber(text:match '([01]+)', 2)
		if num < 256 then
			res['addr'] = 'zpy'
			res['lo'] = num
		else
			res['addr'] = 'aby'
			res['lo'] = num & 0x00ff
			res['hi'] = (num & 0xff00) >> 8
		end

	elseif text:match '^%(%s*$%x+%s*,%s*x%s*%)$' then
		res['addr'] = 'inx'
		res['lo'] = tonumber(text:match '(%x+)', 16)
	elseif text:match '^%(%s*%d+%s*,%s*x%s*%)$' then
		res['addr'] = 'inx'
		res['lo'] = tonumber(text:match '(%d+)')
	elseif text:match '^%(%s*%%[01]+%s*,%s*x%s*%)$' then
		res['addr'] = 'inx'
		res['lo'] = tonumber(text:match '([01]+)', 2)

	elseif text:match '^%(%s*$%x+%s*%)%s*,%s*y$' then
		res['addr'] = 'iny'
		res['lo'] = tonumber(text:match '(%x+)', 16)
	elseif text:match '^%(%s*%d+%s*%)%s*,%s*y$' then
		res['addr'] = 'iny'
		res['lo'] = tonumber(text:match '(%d+)')
	elseif text:match '^%(%s*%%[01]%s*%)%s*,%s*y$' then
		res['addr'] = 'iny'
		res['lo'] = tonumber(text:match '([01]+)', 2)

	elseif text:match '^%(%s*$%x+%s*%)$' then
		local num = tonumber(text:match '(%x+)', 16)
		res['addr'] = 'ind'
		res['lo'] = num & 0x00ff
		res['hi'] = (num & 0xff00) >> 8
	elseif text:match '^%(%s*%d+%s*%)$' then
		local num = tonumber(text:match '(%d+)')
		res['addr'] = 'ind'
		res['lo'] = num & 0x00ff
		res['hi'] = (num & 0xff00) >> 8
	elseif text:match '^%(%s*%%[01]+%s*%)$' then
		local num = tonumber(text:match '([01]+)', 2)
		res['addr'] = 'ind'
		res['lo'] = num & 0x00ff
		res['hi'] = (num & 0xff00) >> 8


	elseif text:match '^#%s*$%x+$' then
		res['addr'] = 'imm'
		local num = tonumber(text:match '(%x+)', 16)
		res['lo'] = num & 0x00ff
		res['hi'] = (num & 0xff00) >> 8
	elseif text:match '^#%s*%d+$' then
		res['addr'] = 'imm'
		local num = tonumber(text:match '(%x+)', 10)
		res['lo'] = num & 0x00ff
		res['hi'] = (num & 0xff00) >> 8
	elseif text:match '^#%s*%%[01]+$' then
		res['addr'] = 'imm'
		local num = tonumber(text:match '(%x+)', 2)
		res['lo'] = num & 0x00ff
		res['hi'] = (num & 0xff00) >> 8
	elseif text:match '^%w+$' then
		res['addr'] = 'rel'
		res['lo'] = text:match '%w+'
	end
	return res
end


function Assemble(code)
	local assembly = {}
	local labels = {}
	-- populate labels
	local counter = 0
	for part in code:gmatch '%s*([^\n\t ]+)%s*' do
		local label = part:match '^:(%w+)$'
		print(part)
		if label ~= nil then
			if labels[label] ~= nil then
				error('Label: ' .. label .. ' is already in use')
			end
			labels[label] = counter
		else
			counter = counter + 1
		end
	end
	-- the rest of the thing
	for line in code:gmatch '%s*([^\n]+)' do
		if line == '' or line == nil then
			-- I hate it here
			goto continue
		end
		-- check if instruction
		if line:find '^%a%a%a' then
			local instr = string.upper(line:match '^%s*(%a%a%a)')
			local addr  = line:match '^%s*%a%a%a%s+(.+)%s*$'
			local addrType = GetAddressing(addr)
			local an = addrType['addr']
			local lo = addrType['lo']
			local hi = addrType['hi']
			-- check if instruction is valid
			if Instructions[instr] == nil then
				error('The instruction ' .. instr .. ' does not exist')
			end
			-- check if addressing mode is valid
			if Instructions[instr][an] == nil then
				error(an .. ' is and invalid addressing mode for ' .. instr)
			end

			table.insert(assembly, Instructions[instr][an])
			-- 1 byte instructions
			-- label things
			if an == 'rel' then
				if type(lo) == "string" then
					table.insert(assembly, labels[lo])
				else
					table.insert(assembly, lo)
				end
			else
				-- 1 byte instructions
				if lo ~= nil then table.insert(assembly, lo) end
				-- 2 byte instructions
				if hi ~= nil then table.insert(assembly, hi) end
			end
		-- label			
		elseif line:match '^%s*$%x+%s*$' then
			table.insert(assembly, tonumber(line:match '(%x+)', 16))
		elseif line:match '^%s*%d+%s*$' then
			table.insert(assembly, tonumber(line:match '(%d+)'))
		elseif line:match '^%s*%%[01]+%s*$' then
			table.insert(assembly, tonumber(line:match '([01]+)', 2))
		end
	    ::continue::
	end
	return assembly
end


Res = Assemble([[
	:test
		adc #$1212
		lda (23,x)
		jmp (2200)
		lda 23
		bcc test
		bcc res
		adc #%011100
	:res
]])

print(table.concat(Res, '\n'))