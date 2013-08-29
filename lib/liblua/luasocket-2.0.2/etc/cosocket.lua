local base = _G
local table = require("table")
local socket = require("socket")
local coroutine = require("coroutine")
local scheduler = require("scheduler")

module("socket.cosocket")

-----------------------------------------------------------------------------
-- socket.tcp() wrapper for the coroutine dispatcher
-----------------------------------------------------------------------------
function cowrap(tcp, error)
    if not tcp then return nil, error end
    -- put it in non-blocking mode right away
    tcp:settimeout(0)
    -- metatable for wrap produces new methods on demand for those that we
    -- don't override explicitly.
    local metat = { __index = function(table, key)
        table[key] = function(...)
            arg[1] = tcp
            return tcp[key](base.unpack(arg))
        end
        return table[key]
    end}
    -- set default timeout to infinity
    local timeout = nil
    -- create a wrap object that will behave just like a real socket object
    local wrap = {  }
    -- we ignore settimeout to preserve our 0 timeout, but record whether
    -- the user wants to do his own non-blocking I/O
    function wrap:settimeout(value, mode)
        timeout = value * 1000 -- convert seconds to milliseconds
        return 1
    end
    -- send in non-blocking mode and yield on timeout
    function wrap:send(data, first, last)
        first = (first or 1) - 1
        local result, error
        while true do
            -- try sending
            result, error, first = tcp:send(data, first+1, last)
            -- if we are done, or there was an unexpected error,
            -- break away from loop
            if error ~= "timeout" then 
                return result, error, first 
              else
                -- return control to scheduler and tell it we want to send
                local ret, err = scheduler.wait(tcp:getfd(), "write", timeout)
                if not ret then
                    return nil, err
                end
            end
        end
    end
    -- receive in non-blocking mode and yield on timeout
    -- or simply return partial read, if user requested timeout = 0
    function wrap:receive(pattern, partial)
        local error = "timeout"
        local value
        while true do
            -- try receiving
            value, error, partial = tcp:receive(pattern, partial)
            -- if we are done, or there was an unexpected error,
            -- break away from loop. also, if the user requested
            -- zero timeout, return all we got
            if (error ~= "timeout") or timeout == 0 then
                return value, error, partial
            else
                -- return control to dispatcher and tell it we want to receive
                local ret, err = scheduler.wait(tcp:getfd(), "read", timeout)
                if not ret then
                    return nil, err
                end
            end
        end
    end
    -- connect in non-blocking mode and yield on timeout
    function wrap:connect(host, port)
        local result, error = tcp:connect(host, port)
        if error == "timeout" then
            -- return control to dispatcher. we will be writable when
            -- connection succeeds.
            local ret, err = scheduler.wait(tcp:getfd(), "write", timeout)
            if not ret then
                return nil, err
            end
            -- when we come back, check if connection was successful
            result, error = tcp:connect(host, port)
            if result or error == "already connected" then return 1
            else return nil, "non-blocking connect failed" end
        else 
            return result, error 
        end
    end
    -- accept in non-blocking mode and yield on timeout
    function wrap:accept()
        while 1 do
            -- return control to dispatcher. we will be readable when a
            -- connection arrives.
            local ret, err = scheduler.wait(tcp:getfd(), "read")
            if not ret then
                return nil, err
            end
            local client, error = tcp:accept()
            if error ~= "timeout" then
                return cowrap(client, error)
            end
        end
    end
    return base.setmetatable(wrap, metat)
end

function tcp()
    return cowrap(socket.tcp())
end

