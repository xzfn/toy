print('hello from toyentry.lua')

local gameapp = require('gameapp')


local app = gameapp.App.new()


local M = {}

function M.startup()
	print('lua startup')
	app:startup()
end

function M.update()
	app:update()
end

function M.shutdown()
	print('lua shutdown')
	app:shutdown()
end

return M
