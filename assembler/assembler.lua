function getAddressing(text)
	local res ={}

	if text == nil then
		res["addr"] = "abs"
		res["out"] = nil
	elseif text:match "^%*%+%d?%d$" then
		res["addr"] = "rel"
		res["out"] = tonumber(text:match "%*(%+%d+)")
	elseif text:match "^%*%-%d?%d$" then
		res["addr"] = "rel"
		res["out"] = tonumber(text:match "%*(%-%d+)")
	-- hex
	elseif text:match "^$%d%d?$" then
		res["addr"] = "zpi" -- also rel
		res["out"] = tonumber(text:match "$(%d+)" , 16)
	elseif text:match "^$%d%d?,x$" then
		res["addr"] = "zpx"
		res["out"] = tonumber(text:match "$(%d+)" , 16)
	elseif text:match "^$%d%d?,y$" then
		res["addr"] = "zpy"
		res["out"] = tonumber(text:match "$(%d+)" , 16)
	elseif text:match "^$%d?%d%d%d$" then
		res["addr"] = "abs"
		res["out"] = {}
		res["out"][1] = tonumber(text:match "$%d?%d(%d%d)", 16)
		res["out"][2] = tonumber(text:match "$(%d?%d)%d%d", 16)
	elseif text:match "^$%d?%d%d%d,%s-x$" then
		res["addr"] = "abx"
		res["out"] = {}
		res["out"][1] = tonumber(text:match "$%d?%d(%d%d)", 16)
		res["out"][2] = tonumber(text:match "$(%d?%d)%d%d", 16)
	elseif text:match "^$%d?%d%d%d,%s-y$" then
		res["addr"] = "aby"
		res["out"] = {}
		res["out"][1] = tonumber(text:match "$%d?%d(%d%d)", 16)
		res["out"][2] = tonumber(text:match "$(%d?%d)%d%d", 16)
	elseif text:match "^%($%d%d,%s-x%)$" then
		res["addr"] = "inx"
		res["out"] = tonumber(text:match "$(%d+)", 16)
	elseif text:match "^%($%d%d%),%s-y$" then
		res["addr"] = "iny"
		res["out"] = tonumber(text:match "$(%d+)", 16)
	elseif text:match "^%($%d?%d%d%d%)$" then
		res["addr"] = "ind"
		res["out"] = tonumber(text:match "$(%d+)", 16)
	elseif text:match "^#%d?%d$" then
		res["addr"] = "imm"
		res["out"] = tonumber(text:match "#(%d+)", 10)
	elseif text:match "^A$" or text:match "^a$" then
		res["addr"] = "acc"
	end
	return res
end

function assemble(line)
	if line == "" or line == nil then
		return nil
	end
	local assembly = {}
	if line:find "%a%a%a" then

		local instr = string.upper(line:match "^%s*(%a%a%a)")
		local addr  = line:match "^%s*%a%a%a%s+(.+)%s*$"
		local addrType = getAddressing(addr)
		local addrName = addrType["addr"]
		local out = addrType["out"]
		-- check if instruction
		if instr:match("%a%a%a") then
			local case = {
				ADC = function ()
					if addrName == 'imm' then
						table.insert(assembly, 0x69)
						table.insert(assembly, out)
					elseif addrName == "zpi" then
						table.insert(assembly, 0x65)
						table.insert(assembly, out)
					elseif addrName == "zpx" then
						table.insert(assembly, 0x75)
						table.insert(assembly, out)
					elseif addrName == "abs" then
						table.insert(assembly, 0x6D)
						table.insert(assembly, out[1])
						table.insert(assembly, out[2])
					elseif addrName == "abx" then
						table.insert(assembly, 0x7D)
						table.insert(assembly, out[1])
						table.insert(assembly, out[2])
					elseif addrName == "aby" then
						table.insert(assembly, 0x79)
						table.insert(assembly, out[1])
						table.insert(assembly, out[2])
					elseif addrName == "inx" then
						table.insert(assembly, 0x61)
						table.insert(assembly, out)
					elseif addrName == "iny" then
						table.insert(assembly, 0x71)
						table.insert(assembly, out)
					end
				end,
				AND = function ()
					if addrName == "imm" then
						table.insert(assembly, 0x29);
						table.insert(assembly, out);
					elseif addrName == "zpi" then
						table.insert(assembly, 0x25)
						table.insert(assembly, out)
					elseif addrName == "zpx" then
						table.insert(assembly, 0x35)
						table.insert(assembly, out)
					elseif addrName == "abs" then
						table.insert(assembly, 0x2D)
						table.insert(assembly, out[1])
						table.insert(assembly, out[2])
					elseif addrName == "abx" then
						table.insert(assembly, 0x3D)
						table.insert(assembly, out[1])
						table.insert(assembly, out[2])
					elseif addrName == "aby" then
						table.insert(assembly, 0x39)
						table.insert(assembly, out[1])
						table.insert(assembly, out[2])
					elseif addrName == "inx" then
						table.insert(assembly, 0x21)
						table.insert(assembly, out)
					elseif addrName == "iny" then
						table.insert(assembly, 0x31)
						table.insert(assembly, out)
					end
				end,
				ASL = function ()
					if addrName == "acc" then
						table.insert(assembly, 0x0A)
					elseif addrName == "zpi" then
						table.insert(assembly, 0x06)
						table.insert(assembly, out)
					elseif addrName == "zpx" then
						table.insert(assembly, 0x16)
						table.insert(assembly, out)
					elseif addrName == "abs" then
						table.insert(assembly, 0x0E)
						table.insert(assembly, out[1])
						table.insert(assembly, out[2])
					elseif addrName == "abx" then
						table.insert(assembly, 0x1E)
						table.insert(assembly, out[1])
						table.insert(assembly, out[2])
					end
				end,
				BCC = function ()
					if addrName == "zpi" then
						table.insert(assembly, 0x90)
						table.insert(assembly, out)
					elseif addrName == "rel" then
						table.insert(assembly, 0x90)
						table.insert(assembly, #assembly + out)
					end
				end

			}
			if case[instr] then
				case[instr]()
			end
		end
	end
	return assembly
end

function main()
	local str = [[
		asl a
		asl A
		asl $1234, x
		bcc *-2
	]]
	local assembly = {}
	for line in str:gmatch("([^\n]+)") do
		local res = assemble(line)
		if res ~= nil then
			for _, i in ipairs(res) do
				table.insert(assembly, i)
			end
		end
	end
	for _, i in ipairs(assembly) do
		print(i)
	end
end



if not package.loaded['assembler'] then
	main()
end