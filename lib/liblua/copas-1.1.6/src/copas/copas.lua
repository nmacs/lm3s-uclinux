-------------------------------------------------------------------------------
-- Copas - Coroutine Oriented Portable Asynchronous Services
--
-- A dispatcher based on coroutines that can be used by TCP/IP servers.
-- Uses LuaSocket as the interface with the TCP/IP stack.
--
-- Authors: Andre Carregal and Javier Guerra
-- Contributors: Diego Nehab, Mike Pall, David Burgess, Leonardo Godinho,
--               Thomas Harning Jr. and Gary NG
--
-- Copyright 2005 - Kepler Project (www.keplerproject.org)
--
-- $Id: copas.lua,v 1.37 2009/04/07 22:09:52 carregal Exp $
-------------------------------------------------------------------------------

if package.loaded["socket.http"] then
  error("you must require copas before require'ing socket.http")
end

local socket  = require "socket"
local syslog  = require "syslog"

local WATCH_DOG_TIMEOUT = 120

-- Redefines LuaSocket functions with coroutine safe versions
-- (this allows the use of socket.http from within copas)
local function statusHandler(status, ...)
	if status then return ... end
 	return nil, ...
end

function socket.protect(func)
	return function (...)
    	return statusHandler(pcall(func, ...))
	end
end

function socket.newtry(finalizer)
	return function (...)
    	local status = (...) or false
        if (status==false)then
			pcall(finalizer, select(2, ...) )
			error((select(2, ...)), 0)
		end
		return ...
	end
end
-- end of LuaSocket redefinitions


module ("copas", package.seeall)

-- Meta information is public even if beginning with an "_"
_COPYRIGHT   = "Copyright (C) 2005 Kepler Project"
_DESCRIPTION = "Coroutine Oriented Portable Asynchronous Services"
_VERSION     = "Copas 1.1.5"

-------------------------------------------------------------------------------
-- Simple set implementation based on LuaSocket's tinyirc.lua example
-- adds a FIFO queue for each value in the set
-------------------------------------------------------------------------------
local function newset()
   local threads = {}
   local threads_reverse = {}
   local reverse = {}
   local set = {}
   local q = {}
   setmetatable(set, { __index = {
       insert = function(set, value, thread)
           if not reverse[value] then
               if thread then
                   threads[thread] = value
                   threads_reverse[value] = thread
               end
               set[#set + 1] = value
               reverse[value] = #set
           end
       end,

       remove_by_value = function(set, value)
           local index = reverse[value]
           if index then
               local thread = threads_reverse[value]
               if thread then
                   threads_reverse[value] = nil
                   threads[thread] = nil
               end
               reverse[value] = nil
               local top = set[#set]
               set[#set] = nil
               if top ~= value then
                   reverse[top] = index
                   set[index] = top
               end
           end
       end,

       remove_by_thread = function(set, thread)
           local value = threads[thread]
           if value then
               set:remove_by_value(value)
               set:pop_all(value)
               return {set=set, value=value}
           end
       end,

       push = function (set, key, itm)
               local qKey = q[key]
               if qKey == nil then
                       q[key] = {itm}
               else
                       qKey[#qKey + 1] = itm
               end
       end,

       pop = function (set, key)
         local t = q[key]
         if t ~= nil then
           local ret = table.remove (t, 1)
           if t[1] == nil then
             q[key] = nil
           end
           return ret
         end
       end,

       pop_all = function (set, key)
           q[key] = nil
       end
   }})
   return set
end

local _reading_log = {}
local _writing_log = {}

local _servers = newset() -- servers being handled
local _reading = newset() -- sockets currently being read
local _writing = newset() -- sockets currently being written
local _waiting = newset() -- just timeouts

-------------------------------------------------------------------------------
-- Coroutine based socket I/O functions.
-------------------------------------------------------------------------------
-- reads a pattern from a client and yields to the reading set on timeouts
function receive(client, pattern, part)
 local s, err
 pattern = pattern or "*l"
 repeat
   s, err, part = client:receive(pattern, part)
   if s or err ~= "timeout" then
       _reading_log[client] = nil
       return s, err, part
   end
   _reading_log[client] = os.time()
   coroutine.yield(client, _reading)
 until false
end

-- same as above but with special treatment when reading chunks,
-- unblocks on any data received.
function receivePartial(client, pattern)
 local s, err, part
 pattern = pattern or "*l"
 repeat
   s, err, part = client:receive(pattern)
   if s or ( (type(pattern)=="number") and part~="" and part ~=nil ) or
      err ~= "timeout" then
       _reading_log[client] = nil
       return s, err, part
   end
   _reading_log[client] = os.time()
   coroutine.yield(client, _reading)
 until false
end

-- sends data to a client. The operation is buffered and
-- yields to the writing set on timeouts
function send(client,data, from, to)
 local s, err,sent
 from = from or 1
 local lastIndex = from - 1

 repeat
   s, err, lastIndex = client:send(data, lastIndex + 1, to)
   if s or err ~= "timeout" then
       _writing_log[client] = nil
       return s, err,lastIndex
   end
   _writing_log[client] = os.time()
   coroutine.yield(client, _writing)
 until false
end

-- waits until connection is completed
function connect(skt, host, port)
	skt:settimeout(0)
	local ret, err
	repeat
		ret, err = skt:connect (host, port)
		if ret or err ~= "timeout" then
			_writing_log[skt] = nil
			return ret, err
		end
		_writing_log[skt] = os.time()
		coroutine.yield(skt, _writing)
	until false
	return ret, err
end

-- flushes a client write buffer (deprecated)
function flush(client)
end

-- wraps a socket to use Copas methods (send, receive, flush and settimeout)
local _skt_mt = {__index = {
       send = function (self, data, from, to)
                       return send (self.socket, data, from, to)
       end,

       receive = function (self, pattern)
                       if (self.timeout==0) then
                               return receivePartial(self.socket, pattern)
                       end
                       return receive (self.socket, pattern)
       end,

       flush = function (self)
                       return flush (self.socket)
       end,

       settimeout = function (self,time)
                       self.timeout=time
                       return
       end,
}}

function wrap (skt)
       return  setmetatable ({socket = skt}, _skt_mt)
end

--------------------------------------------------
-- Error handling
--------------------------------------------------

local _errhandlers = {}   -- error handler per coroutine

function setErrorHandler (err)
       local co = coroutine.running()
       if co then
               _errhandlers [co] = err
       end
end

local function _deferror (msg, co, skt)
       --print (msg, co, skt)
       syslog.syslog("LOG_WARNING", msg)
end

-------------------------------------------------------------------------------
-- Thread handling
-------------------------------------------------------------------------------

local function _doTick (co, skt, ...)
   if not co then return end

   local ok, res, new_q = coroutine.resume(co, skt, ...)
   if ok and res and type(new_q) == "string" then
       if new_q == "reading" then
           new_q = _reading
       elseif new_q == "writing" then
           new_q = _writing
       elseif new_q == "waiting" then
           new_q = _waiting
       end
   end

   if ok and res and new_q then
       if res.timeout then
          res.timestamp = os.clock()
       end
       new_q:insert (res, co)
       new_q:push (res, co)
   else
       if not ok then pcall (_errhandlers [co] or _deferror, res, co, skt) end
       _errhandlers [co] = nil
   end
end

-- accepts a connection on socket input
local function _accept(input, handler)
	local client = input:accept()
	if client then
		client:settimeout(0)
		local co = coroutine.create(handler)
		_doTick (co, client)
	end
	return client
end

-- handle threads on a queue
local function _tickRead (skt)
       if type(skt) == "table" and skt.status == nil then
           skt.status = 1
       end
       _doTick (_reading:pop (skt), skt)
end

local function _tickWrite (skt)
       if type(skt) == "table" and skt.status == nil then
           skt.status = 1
       end
       _doTick (_writing:pop (skt), skt)
end

local function _tickWait (skt)
       _doTick (_waiting:pop (skt), skt)
end

-------------------------------------------------------------------------------
-- Adds a server/handler pair to Copas dispatcher
-------------------------------------------------------------------------------
function addserver(server, handler, timeout)
	server:settimeout(timeout or 0.1)
	_servers[server] = handler
	_reading:insert(server)
end

-------------------------------------------------------------------------------
-- Adds an new courotine thread to Copas dispatcher
-------------------------------------------------------------------------------
function addthread(thread, ...)
       local co = coroutine.create(thread)
       _doTick (co, nil, ...)
end

function newthread(thread)
	return coroutine.create(thread)
end

function runthread(thread, ...)
	_doTick (thread, nil, ...)
end

-------------------------------------------------------------------------------
-- Controls Copas thread
-------------------------------------------------------------------------------

function suspend(thread)
    return _reading:remove_by_thread(thread) or 
           _writing:remove_by_thread(thread) or 
           _waiting:remove_by_thread(thread)
end

function resume_and_wait(thread, operation)
	if not operation then return end
	operation.set:insert(operation.value, thread)
	operation.set:push(operation.value, thread)
end

function resume_and_run(thread)
	local value = {timeout = 0}
	_waiting:insert(value, thread)
	_waiting:push(value, thread)
end

-------------------------------------------------------------------------------
-- Copas wait functions
-------------------------------------------------------------------------------

local function do_wait(set, fd, timeout)
	return coroutine.yield(
		{
			getfd = function () return fd end, 
			timeout = timeout
		}, set)
end

function wait_write(fd, timeout)
	return do_wait(_writing, fd, timeout)
end

function wait_read(fd, timeout)
	return do_wait(_reading, fd, timeout)
end

function wait(timeout)
	return do_wait(_waiting, nil, timeout)
end

-------------------------------------------------------------------------------
-- tasks registering
-------------------------------------------------------------------------------

local _tasks = {}

local function addtaskRead (tsk)
       -- lets tasks call the default _tick()
       tsk.def_tick = _tickRead

       _tasks [tsk] = true
end

local function addtaskWrite (tsk)
       -- lets tasks call the default _tick()
       tsk.def_tick = _tickWrite

       _tasks [tsk] = true
end

local function tasks ()
       return next, _tasks
end

-------------------------------------------------------------------------------
-- main tasks: manage readable and writable socket sets
-------------------------------------------------------------------------------
-- a task to check ready to read events
local _readable_t = {
	events = function(self)
		local i = 0
		return function ()
			i = i + 1
			return self._evs [i]
		end
	end,

	tick = function (self, input)
		local handler = _servers[input]
		if handler then
			input = _accept(input, handler)
		else
			_reading:remove_by_value (input)
			self.def_tick (input)
		end
	end
}

addtaskRead (_readable_t)


-- a task to check ready to write events
local _writable_t = {
	events = function (self)
		local i = 0
		return function ()
			i = i + 1
			return self._evs [i]
		end
	end,

	tick = function (self, output)
		_writing:remove_by_value (output)
		self.def_tick (output)
	end
}

addtaskWrite (_writable_t)

local _waitdone_t = {
	events = function (self)
		local i = 0
		return function ()
			i = i + 1
			return self._evs [i]
		end
	end,

	tick = function (self, output)
		_waiting:remove_by_value (output)
		self.def_tick (output)
	end,

	def_tick = _tickWait
}

local last_cleansing = 0

-------------------------------------------------------------------------------
-- Checks for reads and writes on sockets
-------------------------------------------------------------------------------
local function _get_timeout(set, timeout)
	if timeout == 0 then
	    return timeout
	end
	for i, v in pairs(set) do
		if v.timeout ~= nil then
		    local diff = os.diffclock(os.clock(), v.timestamp)
		    if diff >= v.timeout then
		    	return 0
		    end
		    if timeout == nil or (v.timeout - diff) < timeout then
		        timeout = v.timeout - diff
		    end
		end
	end
	return timeout
end

local function _handle_timeout(set, events, err)
	for i, v in pairs(set) do
		if events[v] == nil and v.timeout ~= nil and os.diffclock(os.clock(), v.timestamp) > v.timeout then
			v.status = 0
			events[#events + 1] = v
			events[v] = #events
		end
	end
end

local function _select (timeout)
	local err
	local readable={}
	local writable={}
	local r={}
	local w={}
	local now = os.time()
	local duration = os.difftime

	timeout = _get_timeout(_reading, timeout)
	timeout = _get_timeout(_writing, timeout)
	timeout = _get_timeout(_waiting, timeout)

	_readable_t._evs, _writable_t._evs, err = socket.select(_reading, _writing, timeout)
	local r_evs, w_evs = _readable_t._evs, _writable_t._evs
	_waitdone_t._evs = {}

	_handle_timeout(_reading, r_evs, err)
	_handle_timeout(_writing, w_evs, err)
	_handle_timeout(_waiting, _waitdone_t._evs, err)

	for ev in _waitdone_t:events() do
		_waitdone_t:tick (ev)
	end

	if duration(now, last_cleansing) > WATCH_DOG_TIMEOUT then
		last_cleansing = now
		for k,v in pairs(_reading_log) do
			if not r_evs[k] and duration(now, v) > WATCH_DOG_TIMEOUT then
				_reading_log[k] = nil
				r_evs[#r_evs + 1] = k
				r_evs[k] = #r_evs
			end
		end

		for k,v in pairs(_writing_log) do
			if not w_evs[k] and duration(now, v) > WATCH_DOG_TIMEOUT then
				_writing_log[k] = nil
				w_evs[#w_evs + 1] = k
				w_evs[k] = #w_evs
			end
		end
	end
	
	if err == "timeout" and #r_evs + #w_evs > 0 then
		return nil
	else
		return err
	end
end


-------------------------------------------------------------------------------
-- Dispatcher loop step.
-- Listen to client requests and handles them
-------------------------------------------------------------------------------
function step(timeout)
	local err = _select (timeout)
	if err == "timeout" then return err end

	if err then
		error(err)
	end

	for tsk in tasks() do
		for ev in tsk:events() do
			tsk:tick (ev)
		end
	end
end

-------------------------------------------------------------------------------
-- Dispatcher endless loop.
-- Listen to client requests and handles them forever
-------------------------------------------------------------------------------
function loop(timeout)
       while true do
               step(timeout)
       end
end
