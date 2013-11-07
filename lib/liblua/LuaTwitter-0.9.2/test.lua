#!/usr/bin/env lua

-- Example usage of the LuaTwitter :)

-- loading module
local twitter = require("twitter")

local APP_KEY = "YourAppConsumerKey"
local APP_SECRET = "YourAppConsumerSecret"

-- if you already have access token and it's secret - pass them :)
-- but if you don't have an access token, then you sholud get it. here is example for obtaining access token
local ACCESS_TOKEN = nil
local ACCESS_TOKEN_SECRET = nil

-- creating client instance
me = twitter.Client( ACCESS_TOKEN, ACCESS_TOKEN_SECRET, APP_KEY, APP_SECRET )

print(me:Version())

-- consumer is the OAuth consumer instance
local r = me.consumer.getRequestToken()

print( string.format("Please visit %s to get the PIN", r.getAuthUrl()) )
print( "Enter the PIN:" )
local PIN = io.stdin:read'*l'

local a = me.consumer.getAccessTokenByPin( PIN, r.getToken() )

me:SetAccessToken( a.getToken(), a.getTokenSecret() )

-- verifying given credentials
ret = me:VerifyCredentials()

if not me:isError(ret) then
   print(string.format("Hello %s!", ret.screen_name))
end

-- comment this line to see result's details
if true then return end

ret = me:HomeTimeline()

print( "Result type: ", type(ret) )

-- printing Twitter result
print( "Result: ")

if type(ret) == "table" then
	for k,v in pairs(ret) do
		if type(v) ~= "table" then
			print(tostring(k) .. " => " .. tostring(v))
		else
			print(k)
			for k2,v2 in pairs(v) do
				if type(v2) ~= "table" then
					print("\t"..tostring(k2) .. " => " .. tostring(v2))
				else
					print("\t"..k2)
					for k3,v3 in pairs(v2) do
						print("\t\t"..tostring(k3) .. " => " .. tostring(v3))
					end
				end
			end
		end
	end
else
	print(ret)
end


