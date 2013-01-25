local json = require('json')
local string = require('string')
local base = _G

module("jsonrpc")

ERR_OK = 0
ERR_PARSE_ERROR      = -32700
ERR_INVALID_REQUEST  = -32600
ERR_METHOD_NOT_FOUND = -32601
ERR_INVALID_PARAMS   = -32602
ERR_INTERNAL_ERROR   = -32603

MAX_REQUEST_LENGTH   = 65536

local methods = {}

function add_method(name, fn)
	methods[name] = fn
end

local function find_method(name)
	return methods[name]
end

local function handle_request(data)
	if base.type(data) ~= "table" then
		return ERR_INVALID_REQUEST, "Request must be an object"
	end

	if not data.method then
		return ERR_INVALID_REQUEST, "Request object must have 'method' item", data.id
	end

	if not data.params then
		return ERR_INVALID_REQUEST, "Request object must have 'params' item", data.id
	end

	if base.type(data.params) ~= "table" then
		return ERR_INVALID_REQUEST, "'params' must be an array", data.id
	end

	if base.type(data.method) ~= "string" then
		return ERR_INVALID_REQUEST, "'method' item must be a string", data.id
	end

	local method = find_method(data.method)
	if not method then
		return ERR_METHOD_NOT_FOUND, "There is no method " .. data.method, data.id
	end

	local res, result = base.pcall(method, base.unpack(data.params))

	if data.id == nil then -- ignore method result for notifications
		return ERR_OK
	end

	if res then
		return ERR_OK, (result or json.null), data.id
	else
		return ERR_INTERNAL_ERROR, result, data.id
	end
end

local function send_error_response(msg, code, id, response)
	local id_str = json.encode(id or json.null)
	response.content = string.format([[{"jsonrpc": "2.0", "error": {"code": %i, "message": "%s"}, "id": %s}]],
			code, msg, id_str)
end

local function send_response(data, id, response)
	if id == nil then -- do not send response for notifications
		return
	end

	local id_str = json.encode(id)
	local data_str = json.encode(data)
	response.content = string.format([[{"jsonrpc": "2.0", "result": %s, "id": %s}]], data_str, id_str)
end

function handle_http(request, response, cap)
	if request.cmd_mth ~= "POST" then
		send_error_response("Reques must use POST method.", ERR_INVALID_REQUEST, nil, response)
		return
	end

	local len = base.tonumber(request.headers["content-length"])
	if len == nil then
		send_error_response("Header content-length must present.", ERR_INVALID_REQUEST, nil, response)
		return
	end

	if len > MAX_REQUEST_LENGTH then
		send_error_response("Request is too long. Max size " .. MAX_REQUEST_LENGTH, ERR_INVALID_REQUEST, nil, response)
		return
	end

	local json_data = request.socket:receive(len)

	local res, data = base.pcall(json.decode, json_data)
	if res then
		local res, data, id = handle_request(data)
		if res ~= 0 then
			send_error_response(data, res, id, response)
		else
			send_response(data, id, response)
		end
	else
		send_error_response("Request parsing error. " .. data, ERR_PARSE_ERROR, nil, response)
	end
end