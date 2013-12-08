-----------------------------------------------------------------------------
-- Xavante HTTP handler
--
-- Authors: Javier Guerra and Andre Carregal
-- Copyright (c) 2004-2007 Kepler Project
--
-- $Id: httpd.lua,v 1.45 2009/08/10 20:00:59 mascarenhas Exp $
-----------------------------------------------------------------------------

local url    = require "socket.url"
local syslog = require "syslog"
local socket = require("socket")
local ssl    = require("ssl")

local try    = socket.try

module ("xavante.httpd", package.seeall)

local _serversoftware = ""
local _authenticate = nil

local _serverports = {}

-- handles the change of string.find in 5.1 to string.match
string.gmatch = string.gmatch or string.gfind

function strsplit (str)
	local words = {}

	for w in string.gmatch (str, "%S+") do
		table.insert (words, w)
	end

	return words
end


-- Manages one connection, maybe several requests
-- params:
--		skt : client socket

local function do_connection (skt)
	skt:setoption ("tcp-nodelay", true)
	local srv, port = skt:getsockname ()
	local req = {
		srv = srv,
		port = port,
		socket = skt,
		serversoftware = _serversoftware
	}

	while read_method (req) do
		local res
		read_headers (req)

		repeat
			req.params = nil
			parse_url (req)
			res = make_response (req)
			if _authenticate then
			   if req.headers["authorization"] == nil or not _authenticate(req) then
		          err_401(req, res)
		          break
		       end
		    end
		until handle_request (req, res) ~= "reparse"

		send_response (req, res)

		if not res.keep_alive then
			break
		end
	end
end

function errorhandler (msg, skt)
	msg = tostring(msg)
	syslog.syslog("LOG_ERR", "  Xavante Error: "..msg)
	skt:send ("HTTP/1.1 502 Bad Gateway\r\n")
	skt:send (string.format ("Date: %s\r\n\r\n", os.date ("!%a, %d %b %Y %H:%M:%S GMT")))
	skt:send (string.format ([[
<html><head><title>Xavante Error!</title></head>
<body>
<h1>Xavante Error!</h1>
<p>%s</p>
</body></html>
]], string.gsub (msg, "\n", "<br/>\n")))
end

function connection (skt)
	local res, msg = pcall(do_connection, skt)
	if not res then
		local res, msg = pcall(errorhandler, msg, skt)
		if not res then
			syslog.syslog("LOG_ERR", "Fail to report error: "..tostring(msg))
		end
	end
	skt:close()
end

local ssl_cfg = {
  protocol = "tlsv1",
  options  = "all",
  verify   = "none",
}

-- Forward calls to the real connection object.
local function reg(conn)
   local mt = getmetatable(conn.raw_sock).__index
   for name, method in pairs(mt) do
      if type(method) == "function" then
         conn[name] = function (self, ...)
                         return method(self.raw_sock, ...)
                      end
      end
   end
   local mt = getmetatable(conn.ssl_sock).__index
   for name, method in pairs(mt) do
      if type(method) == "function" then
         conn[name] = function (self, ...)
                         return method(self.ssl_sock, ...)
                      end
      end
   end
end

local function ssl_wrap(skt, params)
   params = params or {}
   -- Default settings
   for k, v in pairs(ssl_cfg) do
      params[k] = params[k] or v
   end
   -- Force server mode
   params.mode = "server"
   local conn = {}
   conn.raw_sock = skt
   conn.ssl_sock = try(ssl.wrap(conn.raw_sock, params))
   try(conn.ssl_sock:dohandshake())
   -- Forward ssl and socket functions
   setmetatable(conn, {__index = function (self, name)
                                    local method = self.ssl_sock[name]
                                    if type(method) == "function" then
                                    	return function (self, ...)
                                    	          return method(self.ssl_sock, ...)
                                    	       end
                                   	end
                                   	method = self.raw_sock[name]
                                   	if type(method) == "function" then
                                    	return function (self, ...)
                                    	          return method(self.raw_sock, ...)
                                    	       end
                                   	end
                                 end})
   --reg(conn)
   return conn
end

function https_connection (skt, params)
   local ssl_skt, msg = socket.protect(ssl_wrap)(skt, params)
   if not ssl_skt then
      local res, msg2 = pcall(errorhandler, msg, skt)
      if not res then
         syslog.syslog("LOG_ERR", "Fail to report error: "..tostring(msg2).." Original message: "..tostring(msg))
      end
   else
      connection(ssl_skt)
   end
end

-- gets and parses the request line
-- params:
--		req: request object
-- returns:
--		true if ok
--		false if connection closed
-- sets:
--		req.cmd_mth: http method
--		req.cmd_url: url requested (as sent by the client)
--		req.cmd_version: http version (usually 'HTTP/1.1')
function read_method (req)
	local err
	req.cmdline, err = req.socket:receive ()

	if not req.cmdline then return nil end
	req.cmd_mth, req.cmd_url, req.cmd_version = unpack (strsplit (req.cmdline))
	req.cmd_mth = string.upper (req.cmd_mth or 'GET')
	req.cmd_url = req.cmd_url or '/'

	return true
end

-- gets and parses the request header fields
-- params:
--		req: request object
-- sets:
--		req.headers: table of header fields, as name => value
function read_headers (req)
	local headers = {}
	local prevval, prevname

	while 1 do
		local l,err = req.socket:receive ()
		if (not l or l == "") then
			req.headers = headers
			return
		end
		local _,_, name, value = string.find (l, "^([^: ]+)%s*:%s*(.+)")
		name = string.lower (name or '')
		if name then
			prevval = headers [name]
			if prevval then
				value = prevval .. "," .. value
			end
			headers [name] = value
			prevname = name
		elseif prevname then
			headers [prevname] = headers [prevname] .. l
		end
	end
end

function parse_url (req)
	local def_url = string.format ("http://%s%s", req.headers.host or "", req.cmd_url or "")

	req.parsed_url = url.parse (def_url or '')
	req.parsed_url.port = req.parsed_url.port or req.port
	req.built_url = url.build (req.parsed_url)

	req.relpath = url.unescape (req.parsed_url.path)
end


-- sets the default response headers
function default_headers (req)
	return  {
		Date = os.date ("!%a, %d %b %Y %H:%M:%S GMT"),
		Server = _serversoftware,
	}
end

function add_res_header (res, h, v)
    if string.lower(h) == "status" then
        res.statusline = "HTTP/1.1 "..v
    else
        local prevval = res.headers [h]
        if (prevval  == nil) then
            res.headers[h] = v
        elseif type (prevval) == "table" then
            table.insert (prevval, v)
        else
            res.headers[h] = {prevval, v}
        end
    end
end


-- sends the response headers
-- params:
--		res: response object
-- uses:
--		res.sent_headers : if true, headers are already sent, does nothing
--		res.statusline : response status, if nil, sends 200 OK
--		res.headers : table of header fields to send
local function send_res_headers (res)
	if (res.sent_headers) then
		return
	end

	if xavante.cookies then
		xavante.cookies.set_res_cookies (res)
	end

	res.statusline = res.statusline or "HTTP/1.1 200 OK"

	res.socket:send (res.statusline.."\r\n")
	for name, value in pairs (res.headers) do
                if type(value) == "table" then
                  for _, value in ipairs(value) do
                    res.socket:send (string.format ("%s: %s\r\n", name, value))
                  end
                else
                  res.socket:send (string.format ("%s: %s\r\n", name, value))
                end
	end
	res.socket:send ("\r\n")

	res.sent_headers = true;
end

-- sends content directly to client
--		sends headers first, if necesary
-- params:
--		res ; response object
--		data : content data to send
local function send_res_data (res, data)

	if not data or data == "" then
		return
	end

	if not res.sent_headers then
		send_res_headers (res)
	end

	if data then
		if res.chunked then
			res.socket:send (string.format ("%X\r\n", string.len (data)))
			res.socket:send (data)
			res.socket:send ("\r\n")
		else
			res.socket:send (data)
		end
	end
end

function make_response (req)
	local res = {
		req = req,
		socket = req.socket,
		headers = default_headers (req),
		add_header = add_res_header,
		send_headers = send_res_headers,
		send_data = send_res_data,
	}

	return res
end

-- sends prebuilt content to the client
-- 		if possible, sets Content-Length: header field
-- params:
--		req : request object
--		res : response object
-- uses:
--		res.content : content data to send
-- sets:
--		res.keep_alive : if possible to keep using the same connection
function send_response (req, res)

	if res.content then
		if not res.sent_headers then
			if (type (res.content) == "table" and not res.chunked) then
				res.content = table.concat (res.content)
			end
			if type (res.content) == "string" then
				res.headers["Content-Length"] = string.len (res.content)
			end
		end
	else
		if not res.sent_headers then
			res.statusline = "HTTP/1.1 204 No Content"
			res.headers["Content-Length"] = 0
		end
	end

    if res.chunked then
        res:add_header ("Transfer-Encoding", "chunked")
    end

	if res.chunked or ((res.headers ["Content-Length"]) and req.headers ["connection"] == "Keep-Alive")
	then
		res.headers ["Connection"] = "Keep-Alive"
		res.keep_alive = true
	else
		res.keep_alive = nil
	end

	if res.content then
		if type (res.content) == "table" then
			for _,v in ipairs (res.content) do res:send_data (v) end
		else
			res:send_data (res.content)
		end
	else
		res:send_headers ()
	end

	if res.chunked then
		res.socket:send ("0\r\n\r\n")
	end
end

function getparams (req)
	if not req.parsed_url.query then return nil end
	if req.params then return req.params end

	local params = {}
	req.params = params

	for parm in string.gmatch (req.parsed_url.query, "([^&]+)") do
		k,v = string.match (parm, "(.*)=(.*)")
		k = url.unescape (k)
		v = url.unescape (v)
		if k ~= nil then
			if params[k] == nil then
				params[k] = v
			elseif type (params[k]) == "table" then
				table.insert (params[k], v)
			else
				params[k] = {params[k], v}
			end
		end
	end

	return params
end

function err_401 (req, res)
	res.statusline = "HTTP/1.1 401 Not Authorized"
	res.headers ["Content-Type"] = "text/html"
	res.headers ["WWW-Authenticate"] = 'Basic realm="http"'
	res.content = string.format ([[
<!DOCTYPE HTML PUBLIC "-//IETF//DTD HTML 2.0//EN">
<HTML><HEAD>
<TITLE>401 Not Authorized</TITLE>
</HEAD><BODY>
<H1>Not Authorized</H1>
Authorization required.<P>
</BODY></HTML>]], req.built_url);
	return res
end

function err_404 (req, res)
	res.statusline = "HTTP/1.1 404 Not Found"
	res.headers ["Content-Type"] = "text/html"
	res.content = string.format ([[
<!DOCTYPE HTML PUBLIC "-//IETF//DTD HTML 2.0//EN">
<HTML><HEAD>
<TITLE>404 Not Found</TITLE>
</HEAD><BODY>
<H1>Not Found</H1>
The requested URL %s was not found on this server.<P>
</BODY></HTML>]], req.built_url);
	return res
end

function err_403 (req, res)
	res.statusline = "HTTP/1.1 403 Forbidden"
	res.headers ["Content-Type"] = "text/html"
	res.content = string.format ([[
<!DOCTYPE HTML PUBLIC "-//IETF//DTD HTML 2.0//EN">
<HTML><HEAD>
<TITLE>403 Forbidden</TITLE>
</HEAD><BODY>
<H1>Forbidden</H1>
You are not allowed to access the requested URL %s .<P>
</BODY></HTML>]], req.built_url);
	return res
end

function err_405 (req, res)
	res.statusline = "HTTP/1.1 405 Method Not Allowed"
	res.content = string.format ([[
<!DOCTYPE HTML PUBLIC "-//IETF//DTD HTML 2.0//EN">
<HTML><HEAD>
<TITLE>405 Method Not Allowed</TITLE>
</HEAD><BODY>
<H1>Not Found</H1>
The Method %s is not allowed for URL %s on this server.<P>
</BODY></HTML>]], req.cmd_mth, req.built_url);
	return res
end

function redirect (res, d)
	res.headers ["Location"] = d
	res.statusline = "HTTP/1.1 302 Found"
	res.content = "redirect"
end

function newserver (host, port, serversoftware, authenticate)
	local _server = assert(socket.bind(host, port))
	_serversoftware = serversoftware
	_authenticate = authenticate
	local _ip, _port = _server:getsockname()
	_serverports[_port] = true
	return _server
end

function get_ports()
  local ports = {}
  for k, _ in pairs(_serverports) do
    table.insert(ports, tostring(k))
  end
  return ports
end

