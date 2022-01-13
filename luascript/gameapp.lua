
local toy = require('toy')
local vmath = require('vmath')

local drawutil = require('drawutil')


local App = {}
App.__index = App

function App.new()
	local self = {}
	setmetatable(self, App)
	-- init
	self._time = 0.0
	return self
end

function App.startup(self)

end

function App.update(self)
	self._time = self._time + toy.app.delta_time
	local s = math.sin(self._time)
	local c = math.cos(self._time)
	drawutil.draw_line(vmath.Vector3.new(0.0, 1.0, 0.0), vmath.Vector3.new(0.0, c, s), vmath.Vector3.new(0.0, 1.0, 0.0))
	drawutil.draw_line(vmath.Vector3.new(0.0, 1.0, 0.0), vmath.Vector3.new(0.0, s, c), vmath.Vector3.new(c, s, 1.0), 1.0)

end

function App.shutdown(self)

end

return {
	App = App
}
