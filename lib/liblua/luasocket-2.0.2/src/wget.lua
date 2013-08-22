local cosocket = require("socket.cosocket")
local http = require("socket.http")
local ltn12 = require("ltn12")
local io = require("io")
local lfs = require("lfs")
local base = _G
local table = require("table")
local string = require("string")
local os = require("os")

module("socket.wget")

local SYNC_SIZE = 10240

local cancel = false

local function save_to_file(handle)
	local bytes = 0
	return function(chunk, err)
		if cancel then
			--handle:sync()
			handle:close()
			return nil, "cancelled"
		end
		if not chunk then
			--handle:sync()
			handle:close()
			return 1
		else
			local res, err = handle:write(chunk)
			if res then
				bytes = bytes + string.len(chunk)
				if bytes > SYNC_SIZE then
					bytes = 0
					--handle:sync()
				end
			end
			return res, err
		end
	end
end

local function get_remote_size(u)
   local r, c, h = http.request {method = "HEAD", url = u}
   if r ~= nil and c == 200 then
       return base.tonumber(h["content-length"])
   end
end

local function get_local_size(args)
	return lfs.attributes(args.tmp, "size")
end

local function finalize(args)
	local res, err = os.rename(args.tmp, args.path)
	if res == nil then
		if args.error then
			args.error(err)
		end
		return
	end

	if args.success then
		args.success(args.path, args)
	end
end

local function get_file(args, local_sz)
	local mode
	if local_sz then
		mode = "a+b"
	else
		mode = "w+b"
	end

	local file, err = io.open(args.tmp, mode)
	if file == nil then
		return nil, err
	end

	local hdrs
	if local_sz then
		hdrs = { ["Range"] = "bytes=" .. local_sz .. "-" }
	end
	local res, code, s = http.request({
		url = args.url,
		sink = save_to_file(file),
		create = cosocket.tcp,
		headers = hdrs
	})
	if res == nil or (code ~= 200 and code ~= 206) then
		return nil, s or code
	end

	return 1
end

local function download_to_file(args)
	if lfs.attributes(args.path, "size") then
		if args.success then
			args.success(args.path, args)
		end
		return
	end

	if args.tmp == nil then
		args.tmp = args.path .. ".tmp"
	end

	local local_sz = get_local_size(args)
	local remote_sz = get_remote_size(args.url)
	if remote_sz == nil  then
		if args.error then args.error("Fail to request file size") end
		return
	end

	if local_sz and local_sz == remote_sz then
		finalize(args)
		return
	end

	if local_sz and local_sz > remote_sz then
		if args.error then args.error("Local file is larger then remote") end
		os.remove(args.tmp)
		return
	end

	local res, err = get_file(args, local_sz)
	if not res then
		if args.error then args.error(err) end
		return
	end

	finalize(args)
end

local function download_to_memory(args)
	local data = {}

	local res, code, s = http.request({
		url = args.url,
		sink = ltn12.sink.table(data),
		create = cosocket.tcp
	})
	if res == nil or (code ~= 200 and code ~= 206) then
		if args.error then args.error(s or code) end
		return
	end

	if args.success then args.success(table.concat(data), args) end
end

local function do_download(skt, args)
	if args.path then
		download_to_file(args)
	else
		download_to_memory(args)
	end
end

function download(args)
	cancel = false
	do_download(args)
end

function cancel_downloads()
	cancel = true
end

