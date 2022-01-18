
local toy = require('toy')
local vmath = require('vmath')
local keycodes = require('keycodes')
local drawutil = require('drawutil')
local retroreload = require('retroreload')


local App = {}
App.__index = App

function App.new()
	local self = {}
	setmetatable(self, App)
	-- init
	self._time = 0.0
	self._height = 0.0
	return self
end

function App.on_key_down(self, key)
	print('lua on_key_down', key)
	if key == keycodes.VK_F7 then
		retroreload.retroreload_all()
	elseif key == keycodes.VK_F4 then
		print('error')
		call_nil_error()
	elseif key == keycodes.VK_1 then
		drawutil.draw_sphere(vmath.Vector3.new(0.0, 2.0, 0.0), 1.0, vmath.Vector3.new(1.0, 1.0, 0.0), 3.0)
		toy.app.timer_manager:add_timer(1.0, function ()
			drawutil.draw_sphere(vmath.Vector3.new(0.0, 2.5, 0.0), 1.0, vmath.Vector3.new(1.0, 0.0, 0.0), 3.0)
		end)
		self._repeat_timer_id = toy.app.timer_manager:add_repeat_timer(0.2, function ()
			drawutil.draw_sphere(vmath.Vector3.new(0.0, 2.5 + math.random(), 0.0), 1.0, vmath.Vector3.new(1.0, 0.0, 0.0), 0.5)
		end)
	elseif key == keycodes.VK_2 then
		toy.app.timer_manager:cancel_timer(self._repeat_timer_id)
	end
end

function App.startup(self)
	retroreload.retroreload_add_ignores({'toy', 'vmath', 'toyentry'})

	toy.app:bind_key_down(function (key) self:on_key_down(key) end)
end

function App.update(self)
	local delta_time = toy.app.delta_time
	self._time = self._time + toy.app.delta_time
	local s = math.sin(self._time)
	local c = math.cos(self._time)
	drawutil.draw_line(vmath.Vector3.new(0.0, 1.0, 0.0), vmath.Vector3.new(0.0, c, s), vmath.Vector3.new(0.0, 1.0, 0.0))
	drawutil.draw_line(vmath.Vector3.new(0.0, 1.0, 0.0), vmath.Vector3.new(0.0, s, c), vmath.Vector3.new(c, s, 1.0), 1.0)
	drawutil.draw_sphere(vmath.Vector3.new(0.0, s, 0.0), c * 0.2, vmath.Vector3.new(1.0, 0.0, (s + 1.0) * 0.5), 0.2)

	local input_manager = toy.app.input_manager
	if input_manager:get_key(keycodes.VK_NUMPAD8) then
		self._height = self._height + delta_time * 2.0
	end
	if input_manager:get_key(keycodes.VK_NUMPAD2) then
		self._height = self._height + delta_time * (-2.0)
	end
	if input_manager:get_key_down(keycodes.VK_NUMPAD5) then
		self._height = 0.0
	end
	drawutil.draw_sphere(vmath.Vector3.new(0.0, self._height, 0.0), 1.0)

	local camera = toy.app.camera_manager:get_camera()
	local fov_delta = 0.0
	if input_manager:get_key(keycodes.VK_NUMPAD4) then
		fov_delta = 1.0
	end
	if input_manager:get_key(keycodes.VK_NUMPAD6) then
		fov_delta = -1.0
	end
	if fov_delta ~= 0.0 then
		local perspective_data = camera:get_perspective_data()
		perspective_data.fov = perspective_data.fov + fov_delta * delta_time * 0.3
		camera:set_perspective_data(perspective_data)
	end
end

function App.shutdown(self)

end

return {
	App = App
}
