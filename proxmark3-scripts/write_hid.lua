local getopt = require('getopt')

example = "script run write_hid -x 2400a724c4"
author = "Michael Caughey [osok]"

desc =
[[
This automates the writing of HID Prox cards.

Arguments:
	-h		this screen
	-x <hex>	hex to write to card
]]

local HEX = ""

function help()
	print(desc)
	print("Example usage")
	print(example)
end

function main(args)


	local verbose = false
	-- Read the parameters
	for opt, val in getopt.getopt(args, 'hx:') do
		if opt == "h" then help() return end
		if opt == "d" then DEBUG = true end
		if opt == "x" then HEX = val end
	end
	core.console( ('lf hid clone %s'):format(HEX))
	
end


main(args)
