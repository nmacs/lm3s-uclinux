local base      = _G
local core      = require("cron.core")
local scheduler = require("scheduler")
local coroutine = require("coroutine")

module("cron")

local function cron_main()
	while true do
		local next = core.next()
		scheduler.msleep(next)
		core.update()
	end
end
local cron_thread = coroutine.create(cron_main)

function new(callback, name)
	return core.new(callback, name)
end

function set(timer, timeout)
	core.set(timer, timeout)
	scheduler.cancel_wait(cron_thread)
end

function reset(timer)
	core.reset(timer)
end

function start()
	return scheduler.run(cron_thread)
end