local toy = require('toy')
local vmath = require('vmath')

local M = {}

function M.draw_line(position0, position1, color, duration)
	color = color or vmath.Vector3.new(1.0, 0.0, 0.0)
	duration = duration or 0.0

	toy.get_app().timed_geometry_builder:add_line(position0, position1, color, duration)
end

return M
