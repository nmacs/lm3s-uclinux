local base      = _G
local core      = require("cron.core")
local scheduler = require("scheduler")
local coroutine = require("coroutine")
local syslog    = require("syslog")

module("cron")

local function cron_main()
	local next
	while true do
		next = core.next()
		scheduler.msleep(next)
		next = core.next()
		if next == 0 then
			core.update()
		end
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