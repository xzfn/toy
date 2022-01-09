local vmath = require('vmath')
local toy = require('toy')

local M = {}

local i = 0

function M.startup()
	print('lua startup')
	print(toy.get_app().timed_geometry_builder)
end

function M.update()
	print('lua update', toy.get_app().delta_time, i)
	i = i + 1
	if i == 200 then
		print('set_close')
		toy.get_app():set_should_close()
	end
end

function M.shutdown()
	print('lua shutdown')
end

return M
