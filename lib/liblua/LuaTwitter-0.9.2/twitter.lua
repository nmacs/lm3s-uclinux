----------------------------------------------------------------------------------------------------------
-- Title: LuaTwitter.
-- Description: Lua client for Twitter API (visit http://apiwiki.twitter.com/ for API details).
-- Author: Kamil Kapron (kkapron@gmail.com, kamil@kkapron.com).
-- Version: 0.9.2 (2010-12-12)
--
-- Legal: Copyright (C) 2009-2010 Kamil Kapron.
--
--					Permission is hereby granted, free of charge, to any person obtaining a copy
--					of this software and associated documentation files (the "Software"), to deal
--					in the Software without restriction, including without limitation the rights
--					to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
--					copies of the Software, and to permit persons to whom the Software is
--					furnished to do so, subject to the following conditions:
--
--					The above copyright notice and this permission notice shall be included in
--					all copies or substantial portions of the Software.
--
--					THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
--					IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
--					FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
--					AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
--					LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
--					OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
--					THE SOFTWARE.
--
-- Requirements:
-- 	JSON4Lua (http://json.luaforge.net/) (included)
-- 	lua-oauth (https://github.com/DracoBlue/lua-oauth) (included, with my few modifications)
-- 	luacrypto (http://luaforge.net/projects/luacrypto/) (required by lua-oauth)
-- 	lbase64 (http://www.tecgraf.puc-rio.br/~lhf/ftp/lua/#lbase64) (required by lua-oauth)
-- 	lua-curl (http://luaforge.net/projects/lua-curl/) (required by lua-oauth)
-- Encoding: UTF-8.
----------------------------------------------------------------------------------------------------------

---------------------------
-- Imports and dependencies
-- ------------------------
local base = _G

local json = require("json")
local oauth = require("oauth")

local string = require("string")
local table = require("table")
local io = require("io")

local type = type
local print = print
local pairs = pairs
local tostring = tostring
local setmetatable = setmetatable
local pcall = pcall

module("twitter")

-----------------
-- Implementation
-----------------

local BaseUri = "https://api.twitter.com/1.1/"

local AuthUrls = {

   ---------------------
   -- Authorization
   ---------------------

   RequestToken = {
      url = "https://api.twitter.com/oauth/request_token",
      type = "GET",
   },

   Authorize = {
      url = "https://api.twitter.com/oauth/authorize",
      type = "GET",
   },

   AccessToken = {
      url = "https://api.twitter.com/oauth/access_token",
      type = "GET",
   },

   Authenticate = {
      url = "https://api.twitter.com/oauth/authenticate",
      type = "GET",
   }

}

local MethodsMap = {

	---------------------
	-- Search API Methods
	---------------------

	Search = {
		url = "http://search.twitter.com/search",
		type = "GET",
	},

	Trends = {
		url = "http://search.twitter.com/trends",
		type = "GET",
	},

	CurrentTrends = {
		url = "http://search.twitter.com/trends/current",
		type = "GET",
	},

	DailyTrends = {
		url = "http://search.twitter.com/trends/daily",
		type = "GET",
	},

	WeeklyTrends = {
		url = "http://search.twitter.com/trends/weekly",
		type = "GET",
	},

	-------------------
	-- REST API Methods
	-------------------

	HelpTest = {
		url = BaseUri .. "help/test",
		type = "GET",
	},

	-------------------

	PublicTimeline = {
		url = BaseUri .. "statuses/public_timeline",
		type = "GET",
	},

	HomeTimeline = {
		url = BaseUri .. "statuses/home_timeline",
		type = "GET",
		auth = true,
	},

	FriendsTimeline = {
		url = BaseUri .. "statuses/friends_timeline",
		type = "GET",
		auth = true,
	},

	UserTimeline = {
		url = BaseUri .. "statuses/user_timeline",
		type = "GET",
		auth = true,
	},

	Mentions = {
		url = BaseUri .. "statuses/mentions",
		type = "GET",
		auth = true,
	},

	RetweetedByMe = {
		url = BaseUri .. "statuses/retweeted_by_me",
		type = "GET",
		auth = true,
	},

	RetweetedToMe = {
		url = BaseUri .. "statuses/retweeted_to_me",
		type = "GET",
		auth = true,
	},

	RetweetsOfMe = {
		url = BaseUri .. "statuses/retweets_of_me",
		type = "GET",
		auth = true,
	},
	
	-------------------

	ShowStatus = {
		url = BaseUri .. "statuses/show/$id",
		type = "GET",
		auth = true,
	},

	UpdateStatus = {
		url = BaseUri .. "statuses/update",
		type = "POST",
		auth = true,
	},

	DestroyStatus = {
		url = BaseUri .. "statuses/destroy/$id",
		type = "POST",
		auth = true,
	},

	Retweet = {
		url = BaseUri .. "statuses/retweet/$id",
		type = "POST",
		auth = true,
	},

	ShowRetweets = {
		url = BaseUri .. "statuses/retweets/$id",
		type = "GET",
		auth = true,
	},

   RetweetedBy = {
      url = BaseUri .. "statuses/$id/retweeted_by",
      type = "GET",
      auth = true
   },

   RetweetedByIds = {
      url = BaseUri .. "statuses/$id/retweeted_by/ids",
      type = "GET",
      auth = true
   },

	-------------------

	ShowUser = {
		url = BaseUri .. "users/show",
		type = "GET",
	},

	SearchUser = {
		url = BaseUri .. "users/search",
		type = "GET",
		auth = true,
	},

	FriendsWithStatuses = {
		url = BaseUri .. "statuses/friends",
		type = "GET",
		auth = true,
	},

	FollowersWithStatuses = {
		url = BaseUri .. "statuses/followers",
		type = "GET",
		auth = true,
	},

	-------------------

	ReceivedDirectMessages = {
		url = BaseUri .. "direct_messages",
		type = "GET",
		auth = true,
	},

	SentDirectMessages = {
		url = BaseUri .. "direct_messages/sent",
		type = "GET",
		auth = true,
	},

	NewDirectMessage = {
		url = BaseUri .. "direct_messages/new",
		type = "POST",
		auth = true,
	},

	DestroyDirectMessage = {
		url = BaseUri .. "direct_messages/destroy/$id",
		type = "POST",
		auth = true,
	},

	-------------------

	CreateFriendship = {
		url = BaseUri .. "friendships/create/$id",
		type = "POST",
		auth = true,
	},

	DestroyFriendship = {
		url = BaseUri .. "friendships/destroy/$id",
		type = "POST",
		auth = true,
	},

	FriendshipExists = {
		url = BaseUri .. "friendships/exists",
		type = "GET",
		auth = true,
	},

	ShowFriendship = {
		url = BaseUri .. "friendships/show",
		type = "GET",
	},

	-------------------

	FriendsIDs = {
		url = BaseUri .. "friends/ids",
		type = "GET",
		auth = true,
	},

	FollowersIDs = {
		url = BaseUri .. "followers/ids",
		type = "GET",
		auth = true,
	},

	-------------------
	
	VerifyCredentials = {
		url = BaseUri .. "account/verify_credentials",
		type = "GET",
		auth = true,
	},

	RateLimitStatusIP = {
		url = BaseUri .. "account/rate_limit_status",
		type = "GET",
	},

	RateLimitStatusUser = {
		url = BaseUri .. "account/rate_limit_status",
		type = "GET",
		auth = true,
	},

	EndSession = {
		url = BaseUri .. "account/end_session",
		type = "POST",
		auth = true,
	},

	UpdateDeliveryDevice = {
		url = BaseUri .. "account/update_delivery_device",
		type = "POST",
		auth = true,
	},

	UpdateProfileColors = {
		url = BaseUri .. "account/update_profile_colors",
		type = "POST",
		auth = true,
	},

	UpdateProfile = {
		url = BaseUri .. "account/update_profile",
		type = "POST",
		auth = true,
	},

	-------------------
	
	UserFavorites = {
		url = BaseUri .. "favorites",
		type = "GET",
		auth = true
	},

	AddToFavorites = {
		url = BaseUri .. "favorites/create/$id",
		type = "POST",
		auth = true,
	},

	RemoveFromFavorites = {
		url = BaseUri .. "favorites/destroy/$id",
		type = "POST",
		auth = true,
	},
	
	-------------------

	EnableNotifications = {
		url = BaseUri .. "notifications/follow/$id",
		type = "POST",
		auth = true,
	},

	DisableNotifications = {
		url = BaseUri .. "notifications/leave/$id",
		type = "POST",
		auth = true,
	},

	-------------------

	BlockUser = {
		url = BaseUri .. "blocks/create/$id",
		type = "POST",
		auth = true,
	},

	UnBlockUser = {
		url = BaseUri .. "blocks/destroy/$id",
		type = "POST",
		auth = true,
	},

	CheckBlock = {
		url = BaseUri .. "blocks/exists/$id",
		type = "GET",
		auth = true,
	},

	BlockedUsers = {
		url = BaseUri .. "blocks/blocking",
		type = "GET",
		auth = true,
	},

	BlockedUsersIDs = {
		url = BaseUri .. "blocks/blocking/ids",
		type = "GET",
		auth = true,
	},

	-------------------

	ReportSpammer = {
		url = BaseUri .. "report_spam",
		type = "POST",
		auth = true,
	},

	-------------------

	SavedSearches = {
		url = BaseUri .. "saved_searches",
		type = "GET",
		auth = true,
	},
	
	ShowSavedSearch = {
		url = BaseUri .. "saved_searches/show/$id",
		type = "GET",
		auth = true,
	},
	
	CreateSavedSearch = {
		url = BaseUri .. "saved_searches/create",
		type = "POST",
		auth = true,
	},
	
	DestroySavedSearch = {
		url = BaseUri .. "saved_searches/destroy/$id",
		type = "POST",
		auth = true,
	},

	-------------------

	AvailableTrendsLocations = {
		url = BaseUri .. "trends/available",
		type = "GET",
	},

	TrendsForLocation = {
		url = BaseUri .. "trends/$id",
		type = "GET",
	},

	-------------------

	CreateList = {
		url = BaseUri .. "$user/lists",
		type = "POST",
		auth = true,
	},

	UpdateList = {
		url = BaseUri .. "$user/lists/$id",
		type = "POST",
		auth = true,
	},

	GetLists = {
		url = BaseUri .. "$user/lists",
		type = "GET",
		auth = true,
	},

	GetList = {
		url = BaseUri .. "$user/lists/$id",
		type = "GET",
		auth = true,
	},

	DeleteList = {
		url = BaseUri .. "$user/lists/$id",
		type = "POST",
		auth = true,
	},

	GetListStatuses = {
		url = BaseUri .. "$user/lists/$id/statuses",
		type = "GET",
		auth = false,
	},

	GetListMemberships = {
		url = BaseUri .. "$user/lists/memberships",
		type = "GET",
		auth = true,
	},

	GetListSubscriptions = {
		url = BaseUri .. "$user/lists/subscriptions",
		type = "GET",
		auth = true,
	},

	-------------------

	GetListMembers = {
		url = BaseUri .. "$user/$list_id/members",
		type = "GET",
		auth = true,
	},


	AddUserToList = {
		url = BaseUri .. "$user/$list_id/members",
		type = "POST",
		auth = true,
	},

	RemoveUserFromList = {
		url = BaseUri .. "$user/$list_id/members",
		type = "POST",
		auth = true,
	},

	GetListMemberID = {
		url = BaseUri .. "$user/$list_id/members/$id",
		type = "GET",
		auth = true,
	},

	-------------------

	GetListSubscribers = {
		url = BaseUri .. "$user/$list_id/subscribers",
		type = "GET",
		auth = true,
	},


	SubscribeList = {
		url = BaseUri .. "$user/$list_id/subscribers",
		type = "POST",
		auth = true,
	},

	UnsubscribeList = {
		url = BaseUri .. "$user/$list_id/subscribers",
		type = "POST",
		auth = true,
	},

	GetListSubscriberID = {
		url = BaseUri .. "$user/$list_id/subscribers/$id",
		type = "GET",
		auth = true,
	},

}

local DataFormat = ".json"

local function UrlEncode( str )
	if (str) then
		str = string.gsub(str, "\n", "\r\n")
		str = string.gsub( str, "([^%w ])",
			function (c) return string.format("%%%02X", string.byte(c)) end)
		str = string.gsub(str, " ", "+")
	end
	return str
end

--- Not Used since Basic Authentication is disabled. OAuth is recommended.
local function MakeAuthUrl( url, user, pass )
	return string.gsub( url, "http\:\/\/", string.format("http://%s:%s@", user or "", pass or "") )
end

---------------------------------
-- Object for making user queries
---------------------------------
TwitterClient = {}

--- TwitterClient constructor
function TwitterClient:Create()
	o = {}
	setmetatable(o, self)
	self.__index = self
	return o
end

--- TwitterClient version information
function TwitterClient:Version()
	return "0.9.2 (2010-12-12)"
end

--- Set access token
-- I suggest not set the third parameter when you are not sure about given token
function TwitterClient:SetAccessToken( accessToken, accessTokenSecret, dontCheckAccessToken )
	self.accessToken = accessToken
   self.accessTokenSecret = accessTokenSecret
	self.credentialsVerified = dontCheckAccessToken or false
end

--- Making Twitter query
-- Don't use it directly. To call Twitter methods, run proper TwitterClient method.
function TwitterClient:MakeQuery( name, args )

	local method = MethodsMap[name]

	if method then
		local result, code, header
		local queryUrl = method.url
		
		if method.auth then
			if not self.credentialsVerified and name ~= "VerifyCredentials" then
				local res, err = self:VerifyCredentials()
				if not res then
					return nil, err
				end
			end
		end

		if args and (args.id or args.user_id or args.screen_name) and (string.find(queryUrl, "\$id") ~= nil) then
			queryUrl = string.gsub( queryUrl, "\$id", tostring(args.id or args.user_id or args.screen_name) )
			args.id = nil
			args.user_id = nil
			args.screen_name = nil
		end

		if args and args.user and (string.find(queryUrl, "\$user") ~= nil) then
			queryUrl = string.gsub( queryUrl, "\$user", tostring(args.user) )
			args.user = nil
		end

		if args and args.list_id and (string.find(queryUrl, "\$list_id") ~= nil) then
			queryUrl = string.gsub( queryUrl, "\$list_id", tostring(args.list_id) )
			args.user = nil
		end

		queryUrl = queryUrl .. DataFormat

      local params = {}

      if method.auth then
            local access = self.consumer.getAccess( self.accessToken, self.accessTokenSecret )
            if args and type(args) == "table" then
               result, code, header = access.call( queryUrl, method.type, args )
            else
               -- print(queryUrl)
               result, code, header = access.call( queryUrl, method.type, {} )
            end
      else
         if args and type(args) == "table" then
			   for k,v in pairs(args) do
               table.insert(params,string.format("%s=%s", UrlEncode(tostring(k)), UrlEncode(tostring(v))))
			   end
		   end
         params = table.concat(params, "&")
         if method.type == 'GET' then
            queryUrl = string.format( "%s?%s", queryUrl, params )
            queryUrl = string.gsub( queryUrl, "\?$", "" )
            result, code, header = self.consumer.rawGetRequest( queryUrl )
         else
            result, code, header = self.consumer.rawPostRequest( queryUrl, params )
         end
      end
            
		if result ~= nil then
			local decodeOK = true
      	decodeOK, result = pcall( function() return json.decode(result) end )
			if not decodeOK then
				code = -1
				if not result then
					result = "Error: can't decode the response"
				end
			end
		else
			if not code then
				code = -2
			end
		end

		if code == 200 then
			if type(result) ~= "table" then
				return result
			end
			if not result.error then
				return result
			end
		end
		
		if type(result) ~= "table" then
			return nil, tostring(result)
		end
		
		if result.errors then
			local err = ""
			for i, v in base.ipairs(result.errors) do
				if type(v) == "table" then
					err = err .. string.format("%s (%d)", v.message or "unknown error", v.code or -1)
				end
			end
			return nil, err
		end
		
		return nil, string.format("unknown error code %d", code)
	end

end

function TwitterClient:isError( result )
   return ( (type(result) == "table") and (result.errorCode ~= nil) )
end


-- Search Methods
-------------------

--- Returns tweets that match a specified query
-- More information about constructing queries: http://apiwiki.twitter.com/Twitter-Search-API-Method%3A-search
-- @param args.query Required. What is user looking for. Should be given as a user-friendly string, which will be url-encoded by this method automatically. Don't encode it yourself, to avoid double encoding.
-- @param args.callback Optional. If supplied, the response will use the JSONP format with a callback of the given name. NOT TESTED
-- @param args.lang Optional. Restricts tweets to the given language, given by an ISO 639-1 code
-- @param args.rpp Osptional. The number of tweets to return per page, up to a max of 100
-- @param args.page Optional. The page number (starting at 1) to return, up to a max of roughly 1500 results (based on rpp) page
-- @param args.since_id Optional. Returns tweets with status ids greater than the given id
-- @param args.geocode Optional. Returns tweets by users located within a given radius of the given latitude/longitude, where the user's location is taken from their Twitter profile. The parameter value is specified by "latitide,longitude,radius", where radius units must be specified as either "mi" (miles) or "km" (kilometers), ex. "40.757929,-73.985506,25km"
-- @param args.show_user Optional. When "true" (it's a string value, not boolean), prepends "<user>:" to the beginning of the tweet. This is useful for readers that do not display Atom's author field. The default is "false"
-- @return table
function TwitterClient:Search( args )
	args.q, args.query = args.query, nil
	return self:MakeQuery( "Search", args )
end

--- Returns the top ten topics that are currently trending on Twitter.
-- The response includes the time of the request, the name of each trend, and the url to the Twitter Search results page for that topic.
-- @return table
function TwitterClient:Trends()
	return self:MakeQuery( "Trends" )
end

--- Returns the current top 10 trending topics on Twitter.
-- The response includes the time of the request, the name of each trending topic, and query used on Twitter Search results page for that topic.
-- @param exclude Optional. Setting this equal to "hashtags" will remove all hashtags from the trends list.
-- @return table
function TwitterClient:CurrentTrends( exclude )
	return self:MakeQuery( "CurrentTrends", { exclude = exclude } )
end

--- Returns the top 20 trending topics for each hour in a given day.
-- @param start_date Optional. Permits specifying a start date for the report. The date should be formatted YYYY-MM-DD.
-- @param exclude Optional. Setting this equal to hashtags will remove all hashtags from the trends list.
-- @return table
function TwitterClient:DailyTrends( start_date, exclude )
	return self:MakeQuery( "DailyTrends", { date = start_date, exclude = exclude } )
end

--- Returns the top 30 trending topics for each day in a given week.
-- @param start_date Optional. Permits specifying a start date for the report. The date should be formatted YYYY-MM-DD.
-- @param exclude Optional. Setting this equal to hashtags will remove all hashtags from the trends list.
-- @return table
function TwitterClient:WeeklyTrends( start_date, exclude )
	return self:MakeQuery( "WeeklyTrends", { date = start_date, exclude = exclude } )
end


-- Help methods
-------------------

--- Returns the string "ok" in the requested format with a 200 OK HTTP status code.
-- @return string
function TwitterClient:Test()
	return self:MakeQuery( "HelpTest" )
end

-- Timeline methods
-------------------

--- Returns the 20 most recent statuses from non-protected users who have set a custom user icon.
-- The public timeline is cached for 60 seconds so requesting it more often than that is a waste of resources.
-- @return table
function TwitterClient:PublicTimeline()
	return self:MakeQuery( "PublicTimeline" )
end

--- Returns the 20 most recent statuses, including retweets, posted by the authenticating user and that user's friends.
-- This is the equivalent of /timeline/home on the Web.
-- @param args.since_id Optional. Returns only statuses with an ID greater than (that is, more recent than) the specified ID.
-- @param args.max_id Optional. Returns only statuses with an ID less than (that is, older than) or equal to the specified ID.
-- @param args.count Optional. Specifies the number of statuses to retrieve. May not be greater than 200.
-- @param args.page Optional. Specifies the page of results to retrieve.
-- @return table
function TwitterClient:HomeTimeline( args )
	return self:MakeQuery( "HomeTimeline", args )
end

--- Returns the 20 most recent statuses posted by the authenticating user and that user's friends.
-- This is the equivalent of /timeline/home on the Web.
-- @param args.since_id Optional. Returns only statuses with an ID greater than (that is, more recent than) the specified ID.
-- @param args.max_id Optional. Returns only statuses with an ID less than (that is, older than) or equal to the specified ID.
-- @param args.count Optional. Specifies the number of statuses to retrieve. May not be greater than 200.
-- @param args.page Optional. Specifies the page of results to retrieve.
-- @return table
function TwitterClient:FriendsTimeline( args )
	return self:MakeQuery( "FriendsTimeline", args )
end

--- Returns the 20 most recent statuses posted from the authenticating user. It's also possible to request another user's timeline via the id parameter.
-- This is the equivalent of the Web /<user> page for your own user, or the profile page for a third party
-- @param args.id Optional. Specifies the ID or screen name of the user for whom to return the user_timeline.
-- @param args.user_id Optional. Specfies the ID of the user for whom to return the user_timeline. Helpful for disambiguating when a valid user ID is also a valid screen name.
-- @param args.screen_name Optional. Specfies the screen name of the user for whom to return the user_timeline. Helpful for disambiguating when a valid screen name is also a user ID.
-- @param args.since_id Optional. Returns only statuses with an ID greater than (that is, more recent than) the specified ID.
-- @param args.max_id Optional. Returns only statuses with an ID less than (that is, older than) or equal to the specified ID.
-- @param args.count Optional. Specifies the number of statuses to retrieve. May not be greater than 200.
-- @param args.page Optional. Specifies the page of results to retrieve.
-- @return table
function TwitterClient:UserTimeline( args )
	return self:MakeQuery( "UserTimeline", args )
end

--- Returns the 20 most recent mentions (status containing @username) for the authenticating user.
-- @param args.since_id Optional. Returns only statuses with an ID greater than (that is, more recent than) the specified ID.
-- @param args.max_id Optional. Returns only statuses with an ID less than (that is, older than) or equal to the specified ID.
-- @param args.count Optional. Specifies the number of statuses to retrieve. May not be greater than 200.
-- @param args.page Optional. Specifies the page of results to retrieve.
-- @return table
function TwitterClient:Mentions( args )
	return self:MakeQuery( "Mentions", args )
end

--- Returns the 20 most recent retweets posted by the authenticating user.
-- @param args.since_id Optional. Returns only statuses with an ID greater than (that is, more recent than) the specified ID.
-- @param args.max_id Optional. Returns only statuses with an ID less than (that is, older than) or equal to the specified ID.
-- @param args.count Optional. Specifies the number of statuses to retrieve. May not be greater than 200.
-- @param args.page Optional. Specifies the page of results to retrieve.
-- @return table
function TwitterClient:RetweetedByMe( args )
	return self:MakeQuery( "RetweetedByMe", args )
end

--- Returns the 20 most recent retweets posted by the authenticating user's friends.
-- @param args.since_id Optional. Returns only statuses with an ID greater than (that is, more recent than) the specified ID.
-- @param args.max_id Optional. Returns only statuses with an ID less than (that is, older than) or equal to the specified ID.
-- @param args.count Optional. Specifies the number of statuses to retrieve. May not be greater than 200.
-- @param args.page Optional. Specifies the page of results to retrieve.
-- @return table
function TwitterClient:RetweetedToMe( args )
	return self:MakeQuery( "RetweetedToMe", args )
end

--- Returns the 20 most recent tweets of the authenticated user that have been retweeted by others.
-- @param args.since_id Optional. Returns only statuses with an ID greater than (that is, more recent than) the specified ID.
-- @param args.max_id Optional. Returns only statuses with an ID less than (that is, older than) or equal to the specified ID.
-- @param args.count Optional. Specifies the number of statuses to retrieve. May not be greater than 200.
-- @param args.page Optional. Specifies the page of results to retrieve.
-- @return table
function TwitterClient:RetweetsOfMe( args )
	return self:MakeQuery( "RetweetsOfMe", args )
end


-- Status methods
------------------

--- Returns a single status, specified by the id parameter below.  The status's author will be returned inline.
-- @param id Required. The numerical ID of the status to retrieve.
-- @return table
function TwitterClient:ShowStatus( id )
	return self:MakeQuery( "ShowStatus", {id = id} )
end

--- Updates the authenticating user's status. Requires the status parameter specified below.
-- A status update with text identical to the authenticating user's current status will be ignored to prevent duplicates.
-- Details about parameters: http://apiwiki.twitter.com/Twitter-REST-API-Method:-statuses update
-- @param status Required The text of your status update. URL encode as necessary. Statuses over 140 characters will be forceably truncated
-- @param in_reply_to_status_id Optional. The ID of an existing status that the update is in reply to
-- @param lat Optional. The location's latitude that this tweet refers to.
-- @param long Optional. The location's longitude that this tweet refers to.
-- @return table
function TwitterClient:UpdateStatus( status, in_reply_to_status_id, lat, long )
	return self:MakeQuery( "UpdateStatus", {status = status, in_reply_to_status_id = in_reply_to_status_id, lat=lat, long=long} )
end

--- Destroys the status specified by the required ID parameter. The authenticating user must be the author of the specified status.
-- @param id Required. The ID of the status to destroy.
-- @return table
function TwitterClient:DestroyStatus( id )
	return self:MakeQuery( "DestroyStatus", {id = id} )
end

--- Retweets a tweet. Requires the id parameter of the tweet you are retweeting.
-- Returns the original tweet with retweet details embedded.
-- @param id. Required. The numerical ID of the tweet you are retweeting.
-- @return table
function TwitterClient:Retweet( id )
	return self:MakeQuery( "Retweet", {id = id} )
end

--- Returns up to 100 of the first retweets of a given tweet.
-- @param id. Required. The numerical ID of the tweet you want the retweets of.
-- @param count Optional. Specifies the number of retweets to retrieve. May not be greater than 100.
-- @return table
function TwitterClient:ShowRetweets( id, count )
	return self:MakeQuery( "ShowRetweets", {id = id, count = count} )
end

--- Show user objects of up to 100 members who retweeted the status.
-- @param id. Required. The numerical ID of the tweet.
-- @param count Optional. Specifies the number of records to retrieve. May not be greater than 100.
-- @param page Optional. Specifies the page of results to retrieve.
-- @param trim_user Optional. When set to either true, t or 1, each tweet returned in a timeline will include a user object including only the status authors numerical ID. Omit this parameter to receive the complete user object.
-- @param entieties Optional. When set to either true, t or 1, each tweet will include a node called "entities,". This node offers a variety of metadata about the tweet in a discreet structure, including: user_mentions, urls, and hashtags. While entities are opt-in on timelines at present, they will be made a default component of output in the future.
-- @return table
function TwitterClient:RetweetedBy( id, count, page, trim_user, entities )
	return self:MakeQuery( "RetweetedBy", {id = id, count = count, page = page, trim_user = trim_user, include_entities = entities} )
end

--- Show user ids of up to 100 users who retweeted the status.
-- @param count Optional. Specifies the number of records to retrieve. May not be greater than 100.
-- @param page Optional. Specifies the page of results to retrieve.
-- @param trim_user Optional. When set to either true, t or 1, each tweet returned in a timeline will include a user object including only the status authors numerical ID. Omit this parameter to receive the complete user object.
-- @param entieties Optional. When set to either true, t or 1, each tweet will include a node called "entities,". This node offers a variety of metadata about the tweet in a discreet structure, including: user_mentions, urls, and hashtags. While entities are opt-in on timelines at present, they will be made a default component of output in the future.
-- @return table
function TwitterClient:RetweetedByIds( id, count, page, trim_user, entities )
	return self:MakeQuery( "RetweetedByIds", {id = id, count = count, page = page, trim_user = trim_user, include_entities = entities} )
end


-- User methods
------------------

--- Returns extended information of a given user, specified by ID or screen name as per the required id parameter.
-- The author's most recent status will be returned inline.
-- One of the following parameters is required:
-- @param args.id The ID or screen name of a user
-- @param args.user_id Specfies the ID of the user to return. Helpful for disambiguating when a valid user ID is also a valid screen name
-- @param args.screen_name Specfies the screen name of the user to return. Helpful for disambiguating when a valid screen name is also a user ID
-- @return table
function TwitterClient:ShowUser( args )
	return self:MakeQuery( "ShowUser", args )
end

--- Run a search for users similar to Find People button on Twitter.com; the same results returned by people search on Twitter.com will be returned by using this API.
-- It is only possible to retrieve the first 1000 matches from this API.
-- @param query Required. The query to run against people search.
-- @param per_page Optional. Specifies the number of statuses to retrieve. May not be greater than 20.
-- @param page Optional. Specifies the page of results to retrieve.
function TwitterClient:SearchUser( query, per_page, page )
	return self:MakeQuery( "SearchUser", {q = query, per_page = per_page, page = page} )
end

--- Returns a user's friends, each with current status inline. They are ordered by the order in which the user followed them, most recently followed first, 100 at a time.
-- (Please note that the result set isn't guaranteed to be 100 every time as suspended users will be filtered out.)
-- Use the cursor option to access older friends. With no user specified, request defaults to the authenticated user's friends.
-- It's also possible to request another user's friends list via the id, screen_name or user_id parameter.
-- @param args.id Optional. The ID or screen name of the user for whom to request a list of friends
-- @param args.user_id Optional. Specfies the ID of the user for whom to return the list of friends. Helpful for disambiguating when a valid user ID is also a valid screen name
-- @param args.screen_name Optional. Specfies the screen name of the user for whom to return the list of friends. Helpful for disambiguating when a valid screen name is also a user ID
-- @param args.cursor Optional. Breaks the results into pages. A single page contains 100 users. This is recommended for users who are following many users. Provide a value of  -1 to begin paging. Provide values as returned to in the response body's next_cursor and previous_cursor attributes to page back and forth in the list.
-- @return table
function TwitterClient:UserFriends( args )
	return self:MakeQuery( "FriendsWithStatuses", args )
end

--- Returns the authenticating user's followers, each with current status inline.
-- They are ordered by the order in which they followed the user, 100 at a time. (Please note that the result set isn't guaranteed to be 100 every time as suspended users will be filtered out.)
-- Use the cursor option to access earlier followers
-- @param args.id Optional. The ID or screen name of the user for whom to request a list of followers
-- @param args.user_id Optional. Specfies the ID of the user for whom to return the list of followers. Helpful for disambiguating when a valid user ID is also a valid screen name
-- @param args.screen_name Optional. Specfies the screen name of the user for whom to return the list of friends. Helpful for disambiguating when a valid screen name is also a user ID
-- @param args.cursor Optional. Breaks the results into pages. A single page contains 100 users. This is recommended for users who are followed by many other users. Provide a value of  -1 to begin paging. Provide values as returned to in the response body's next_cursor and previous_cursor attributes to page back and forth in the list.
-- @return table
function TwitterClient:UserFollowers( args )
	return self:MakeQuery( "FollowersWithStatuses", args )
end


-- Direct Messages methods
------------------

--- Returns a list of the 20 most recent direct messages sent to the authenticating user.
-- @param args.since_id Optional. Returns only direct messages with an ID greater than (that is, more recent than) the specified ID.
-- @param args.max_id Optional. Returns only direct messages with an ID less than (that is, older than) or equal to the specified ID.
-- @param args.count Optional. Specifies the number of direct messages to retrieve. May not be greater than 200.
-- @param args.page Optional. Specifies the page of results to retrieve.
-- @return table
function TwitterClient:ReceivedDirectMessages( args )
	return self:MakeQuery( "ReceivedDirectMessages", args )
end

--- Returns a list of the 20 most recent direct messages sent by the authenticating user.
-- @param args.since_id Optional. Returns only direct messages with an ID greater than (that is, more recent than) the specified ID.
-- @param args.max_id Optional. Returns only direct messages with an ID less than (that is, older than) or equal to the specified ID.
-- @param args.count Optional. Specifies the number of direct messages to retrieve. May not be greater than 200.
-- @param args.page Optional. Specifies the page of results to retrieve.
-- @return table
function TwitterClient:SentDirectMessages( args )
	return self:MakeQuery( "SentDirectMessages", args )
end

--- Sends a new direct message to the specified user from the authenticating user. Requires both the user and text parameters.
-- Returns the sent message in the requested format when successful.
-- @param user Required. The ID or screen name of the recipient user. In order to support numeric screen names Twitter will accept either of the following two parameters in place of the user parameter: screen_name (screen name of the recipient user), user_id (user id of the recipient user)
-- @param text Required. The text of your direct message. Keep it under 140 characters.
-- @return table
function TwitterClient:NewDirectMessage( user, text )
	return self:MakeQuery( "NewDirectMessage", { screen_name = user, text = text } )
end

--- Destroys the direct message specified in the required ID parameter. The authenticating user must be the recipient of the specified direct message.
-- @param id Required. The ID of the direct message to destroy.
-- @return table
function TwitterClient:DestroyDirectMessage( id )
	return self:MakeQuery( "DestroyDirectMessage", { id = id } )
end


-- Friendship Methods Methods
-----------------------------

--- Allows the authenticating users to follow the user specified in the ID parameter.
-- Returns the befriended user in the requested format when successful.  Returns a string describing the failure condition when unsuccessful.
-- If you are already friends with the user an HTTP 403 will be returned.
-- One of the following parameters is required:
-- @param args.id Required. The ID or screen name of the user to befriend.
-- @param args.user_id Required. Specfies the ID of the user to befriend. Helpful for disambiguating when a valid user ID is also a valid screen name.
-- @param args.screen_name Required. Specfies the screen name of the user to befriend. Helpful for disambiguating when a valid screen name is also a user ID.
-- @param args.follow Optional. Enable delivery of statuses from this user to the authenticated user's device; tweets sent by the user being followed will be delivered to the caller of this endpoint's phone as SMSes.
-- @return table
function TwitterClient:FollowUser( args )
	return self:MakeQuery( "CreateFriendship", args )
end

--- Allows the authenticating users to unfollow the user specified in the ID parameter.
-- Returns the unfollowed user in the requested format when successful.  Returns a string describing the failure condition when unsuccessful.
-- One of the following parameters is required:
-- @param args.id Required. The ID or screen name of the user to unfollow.
-- @param args.user_id Required. Specfies the ID of the user to unfollow. Helpful for disambiguating when a valid user ID is also a valid screen name.
-- @param args.screen_name Required. Specfies the screen name of the user to unfollow. Helpful for disambiguating when a valid screen name is also a user ID.
-- @return table
function TwitterClient:UnFollowUser( args )
	return self:MakeQuery( "DestroyFriendship", args )
end

--- Tests for the existence of friendship between two users. Will return true if user_a follows user_b, otherwise will return false.
-- Tip: Why not try the ShowFriendship method, which gives you even more information with a single call?
-- @param user_a Required. The ID or screen_name of the subject user.
-- @param user_b Required. The ID or screen_name of the user to test for following.
-- @return boolean
function TwitterClient:FriendshipExists( user_a, user_b )
	return self:MakeQuery( "FriendshipExists", { user_a = user_a, user_b = user_b } )
end

--- Returns detailed information about the relationship between two users.
-- Usage notes: http://apiwiki.twitter.com/Twitter-REST-API-Method:-friendships-show
-- One of the following parameters is required if the request is unauthenticated:
-- @param args.source_id The user_id of the subject user
-- @param args.source_screen_name The screen_name of the subject user.
-- One of the following parameters is required:
-- @param args.target_id The user_id of the target user.
-- @param args.target_screen_name The screen_name of the target user.
-- @return table
function TwitterClient:ShowFriendship( args )
	return self:MakeQuery( "ShowFriendship", args )
end


-- Social Graph Methods
-----------------------

--- Returns an array of numeric IDs for every user the specified user is following.
-- One of the three following parameters should be given:
-- @param args.id Optional. The ID or screen_name of the user to retrieve the friends ID list for.
-- @param args.user_id Optional. Specfies the ID of the user for whom to return the friends list. Helpful for disambiguating when a valid user ID is also a valid screen name.
-- @param args.screen_name Optional. Specfies the screen name of the user for whom to return the friends list. Helpful for disambiguating when a valid screen name is also a user ID.
-- @param args.cursor Required. Breaks the results into pages. A single page contains 5000 identifiers. This is recommended for all purposes. Provide a value of -1 to begin paging. Provide values as returned to in the response body's next_cursor and previous_cursor attributes to page back and forth in the list.
-- @return table
function TwitterClient:UserFriendsIDs( args )
	if args and not args.cursor then
		args.cursor = -1
	end
	return self:MakeQuery( "FriendsIDs", args )
end

--- Returns an array of numeric IDs for every user following the specified user.
-- One of the three following parameters should be given:
-- @param args.id Optional. The ID or screen_name of the user to retrieve the followers ID list for.
-- @param args.user_id Optional. Specfies the ID of the user for whom to return the friends list. Helpful for disambiguating when a valid user ID is also a valid screen name.
-- @param args.screen_name Optional. Specfies the screen name of the user for whom to return the friends list. Helpful for disambiguating when a valid screen name is also a user ID.
-- @param args.cursor Required. Breaks the results into pages. A single page contains 5000 identifiers. This is recommended for all purposes. Provide a value of -1 to begin paging. Provide values as returned to in the response body's next_cursor and previous_cursor attributes to page back and forth in the list.
-- @return table
function TwitterClient:UserFollowersIDs( args )
	if args and not args.cursor then
		args.cursor = -1
	end
	return self:MakeQuery( "FollowersIDs", args )
end


-- Account methods
------------------

--- Returns an HTTP 200 OK response code and a representation of the requesting user if authentication was successful; returns a 401 status code and an error message if not. Use this method to test if supplied user credentials are valid
-- @return table extended user information element
function TwitterClient:VerifyCredentials()
	local res, err = self:MakeQuery( "VerifyCredentials" )
	if res then
		if type(res) ~= "table" then
			return nil, "VerifyCredentials response should be an object"
		end
		self.credentialsVerified = true
        self.UserID = res.id
	end
	return res, err
end

--- Returns the remaining number of API requests available to the requesting user before the API limit is reached for the current hour.
-- Calls to rate_limit_status do not count against the rate limit. If authentication credentials are provided, the rate limit status for the authenticating user is returned.
-- Otherwise, the rate limit status for the requester's IP address is returned.
-- @param forUser Optional.
-- @return table
function TwitterClient:RateLimitStatus( forUser )
	if forUser then
		return self:MakeQuery( "RateLimitStatusUser" )
	end
	return self:MakeQuery( "RateLimitStatusIP" )
end

--- Ends the session of the authenticating user, returning a null cookie. Use this method to sign users out of client-facing applications like widgets
function TwitterClient:EndSession()
	return self:MakeQuery( "EndSession" )
end

--- Sets which device Twitter delivers updates to for the authenticating user.
-- Sending none as the device parameter will disable IM or SMS updates.
-- @param device Required. Must be one of: sms, none.
-- @return table
function TwitterClient:UpdateDeliveryDevice( dev )
	return self:MakeQuery( "UpdateDeliveryDevice", { device = dev } )
end

--- Sets one or more hex values that control the color scheme of the authenticating user's profile page on twitter.com.
-- One or more of the following parameters must be present. Each parameter's value must be a valid hexidecimal value, and may be either three or six characters (ex: fff or ffffff).
-- @param colors.background Optional
-- @param colors.text Optional
-- @param colors.link Optional
-- @param colors.sidebar_fill Optional
-- @param colors.sidebar_border Optional
-- @return table
function TwitterClient:UpdateProfileColors( colors )
	return self:MakeQuery( "UpdateProfileColors", {
			profile_background_color = colors.background,
			profile_text_color = colors.text,
			profile_link_color = colors.link,
			profile_sidebar_fill_color = colors.sidebar_fill,
			profile_sidebar_border_color = colors.sidebar_border,
	} )
end

--- Sets values that users are able to set under the "Account" tab of their settings page. Only the parameters specified will be updated.
-- One or more of the following parameters must be present. Each parameter's value should be a string. See the individual parameter descriptions below for further constraints.
-- @param args.name Optional. Maximum of 20 characters
-- @param args.url Optional. Maximum of 100 characters. Will be prepended with "http://" if not present
-- @param args.location Optional. Maximum of 30 characters. The contents are not normalized or geocoded in any way
-- @param args.description Optional. Maximum of 160 characters
-- @return table
function TwitterClient:UpdateProfile( args )
	return self:MakeQuery( "UpdateProfile", args )
end


-- Favorite Methods
-------------------

--- Returns the 20 most recent favorite statuses for the authenticating user or user specified by the ID parameter in the requested format.
-- @param id Optional. The ID or screen name of the user for whom to request a list of favorite statuses.
-- @param page Optional. Specifies the page of favorites to retrieve.
-- @return table
function TwitterClient:UserFavorites( id, page )
	return self:MakeQuery( "UserFavorites", { id = id, page = page } )
end

--- Favorites the status specified in the ID parameter as the authenticating user. Returns the favorite status when successful.
-- @param id Required. The ID of the status to favorite.
-- @return table
function TwitterClient:AddToFavorites( id )
	return self:MakeQuery( "AddToFavorites", { id = id } )
end

--- Un-favorites the status specified in the ID parameter as the authenticating user. Returns the un-favorited status in the requested format when successful.
-- @param id Required. The ID of the status to un-favorite.
-- @return table
function TwitterClient:RemoveFromFavorites( id )
	return self:MakeQuery( "RemoveFromFavorites", { id = id } )
end


-- Notification Methods
-----------------------

--- Enables device notifications for updates from the specified user. Returns the specified user when successful.
-- One of the following is required:
-- @param args.id Required. The ID or screen name of the user to follow with device updates.
-- @param args.user_id Required. Specfies the ID of the user to follow with device updates. Helpful for disambiguating when a valid user ID is also a valid screen name.
-- @param args.screen_name Required. Specfies the screen name of the user to follow with device updates. Helpful for disambiguating when a valid screen name is also a user ID.
-- @return table
function TwitterClient:EnableNotifications( args )
	return self:MakeQuery( "EnableNotifications", args )
end

--- Disables notifications for updates from the specified user to the authenticating user. Returns the specified user when successful.
-- One of the following is required:
-- @param args.id Required. The ID or screen name of the user to disable device notifications.
-- @param args.user_id Required. Specfies the ID of the user to disable device notifications. Helpful for disambiguating when a valid user ID is also a valid screen name.
-- @param args.screen_name Required. Specfies the screen name of the user of the user to disable device notifications. Helpful for disambiguating when a valid screen name is also a user ID.
-- @return table
function TwitterClient:DisableNotifications( args )
	return self:MakeQuery( "DisableNotifications", args )
end


-- Block Methods
----------------

--- Blocks the user specified in the ID parameter as the authenticating user. Destroys a friendship to the blocked user if it exists.
-- Returns the blocked user in the requested format when successful.
-- One of the following is required:
-- @param args.id Optional. The ID or screen_name of the potentially blocked user.
-- @param args.user_id Optional. Specfies the ID of the potentially blocked user. Helpful for disambiguating when a valid user ID is also a valid screen name.
-- @param args.screen_name Optional. Specfies the screen name of the potentially blocked user. Helpful for disambiguating when a valid screen name is also a user ID.
-- @return table
function TwitterClient:BlockUser( args )
	return self:MakeQuery( "BlockUser", args )
end

--- Un-blocks the user specified in the ID parameter for the authenticating user.  Returns the un-blocked user in the requested format when successful.
-- One of the following is required:
-- @param args.id Optional. The ID or screen_name of the potentially blocked user.
-- @param args.user_id Optional. Specfies the ID of the potentially blocked user. Helpful for disambiguating when a valid user ID is also a valid screen name.
-- @param args.screen_name Optional. Specfies the screen name of the potentially blocked user. Helpful for disambiguating when a valid screen name is also a user ID.
-- @return table
function TwitterClient:UnBlockUser( args )
	return self:MakeQuery( "UnBlockUser", args )
end

--- Returns if the authenticating user is blocking a target user. Will return the blocked user's object if a block exists, and error with a HTTP 404 response code otherwise.
-- @param args.id Optional. The ID or screen_name of the potentially blocked user.
-- @param args.user_id Optional. Specfies the ID of the potentially blocked user. Helpful for disambiguating when a valid user ID is also a valid screen name.
-- @param args.screen_name Optional. Specfies the screen name of the potentially blocked user. Helpful for disambiguating when a valid screen name is also a user ID.
-- @return table
function TwitterClient:CheckBlock( args )
	return self:MakeQuery( "CheckBlock", args )
end

--- Returns an array of user objects that the authenticating user is blocking.
-- @param page Optional. Specifies the page number of the results beginning at 1. A single page contains 20 ids.
-- @return table
function TwitterClient:BlockedUsers( page )
	return self:MakeQuery( "BlockedUsers", { page = page } )
end

--- Returns an array of numeric user ids the authenticating user is blocking.
-- @param page Optional. Specifies the page number of the results beginning at 1. A single page contains 20 ids.
-- @return table
function TwitterClient:BlockedUsersIDs( page )
	return self:MakeQuery( "BlockedUsersIDs", { page = page } )
end


-- Spam Reporting Methods
-------------------------

--- The user specified in the id is blocked by the authenticated user and reported as a spammer.
-- @param args.id Optional. The ID or screen_name of the user you want to report as a spammer.
-- @param args.user_id Optional. The ID of the user you want to report as a spammer. Helpful for disambiguating when a valid user ID is also a valid screen name.
-- @param args.screen_name Optional. The ID or screen_name of the user you want to report as a spammer. Helpful for disambiguating when a valid screen name is also a user ID.
-- @return table
function TwitterClient:ReportSpammer( args )
	return self:MakeQuery( "ReportSpammer", args )
end


-- Saved Searches Methods
-------------------------

--- Returns the authenticated user's saved search queries.
-- @return table
function TwitterClient:SavedSearches()
	return self:MakeQuery( "SavedSearches" )
end

--- Retrieve the data for a saved search owned by the authenticating user specified by the given id.
-- @param id Required. The id of the saved search to be retrieved.
-- @return table
function TwitterClient:ShowSavedSearch( id )
	return self:MakeQuery( "ShowSavedSearch", { id = id } )
end

--- Creates a saved search for the authenticated user.
-- @param query Required. The query of the search the user would like to save.
-- @return table
function TwitterClient:CreateSavedSearch( query )
	return self:MakeQuery( "CreateSavedSearch", { query = query } )
end

--- Destroys a saved search for the authenticated user. The search specified by id must be owned by the authenticating user.
-- @param id Required. The id of the saved search to be deleted.
-- @return table
function TwitterClient:DestroySavedSearch( id )
	return self:MakeQuery( "DestroySavedSearch", { id = id } )
end


-- Local Trends Methods
-----------------------

--- Returns the locations that Twitter has trending topic information for.
-- The response is an array of "locations" that encode the location's WOEID (a Yahoo! Where On Earth ID)
-- and some other human-readable information such as a canonical name and country the location belongs in.
-- The WOEID that is returned in the location object is to be used when querying for a specific trend.
-- @param lat Optional. If passed in conjunction with long, then the available trend locations will be sorted by distance to the lat and long passed in. The sort is nearest to furthest.
-- @param long Optional. See lat
-- @return table
function TwitterClient:AvailableTrendsLocations( lat, long )
	return self:MakeQuery( "AvailableTrendsLocations", { lat = lat, long = long } )
end

--- Returns the top 10 trending topics for a specific location Twitter has trending topic information for.
-- The response is an array of "trend" objects that encode the name of the trending topic,
-- the query parameter that can be used to search for the topic on Search, and the direct URL that can be issued against Search.
-- This information is cached for five minutes, and therefore users are discouraged from querying these endpoints faster than once every five minutes.
-- Global trends information is also available from this API by using a WOEID of 1.
-- @param woeid The WOEID of the location to be querying for
-- @return table
function TwitterClient:TrendsForLocation( woeid )
	return self:MakeQuery( "TrendsForLocation", { id = woeid } )
end


-- Lists Methods
-----------------------

--- Creates a new list for the authenticated user. Accounts are limited to 20 lists.
-- @param name Required. The name of the list you are creating.
-- @param mode Optional. Whether your list is public or private. Values can be public or private. Lists are public by default if no mode is specified.
-- @param desc Optional. The description of the list you are creating.
-- @return table
function TwitterClient:CreateList( name, mode, desc )
	return self:MakeQuery( "CreateList", { user = self.UserName, name = name, mode = mode, description = desc } )
end

--- Updates the specified list.
-- @param name Required. List id.
-- @param name Optional. What you'd like to change the lists name to.
-- @param mode Optional. Whether your list is public or private. Values can be public or private. Lists are public by default if no mode is specified.
-- @param desc Optional.  What you'd like to change the list description to.
-- @return table
function TwitterClient:UpdateList( id, name, mode, desc )
	return self:MakeQuery( "UpdateList", { user = self.UserName, id = id, name = name, mode = mode, description = desc } )
end

--- List the lists of the specified user. Private lists will be included if the authenticated users is the same as the user who'se lists are being returned.
-- @param user Optional. ID, screen name or user ID of wanted user. Default: authenticated user.
-- @param cursor Optional. Breaks the results into pages. A single page contains 20 lists. Provide a value of -1 to begin paging. Provide values as returned to in the response body's next_cursor and previous_cursor attributes to page back and forth in the list.
-- @return table
function TwitterClient:GetLists( user, cursor )
	if not user then
		user = self.UserName
	end
	if not cursor then
		cursor = "-1"
	end
	return self:MakeQuery( "GetLists", { user = user, cursor = cursor } )
end

--- Show the specified list. Private lists will only be shown if the authenticated user owns the specified list.
-- @param id Required. The id or slug of the list.
-- @param user Optional. ID, screen name or user ID of wanted user. Default: authenticated user.
-- @return table
function TwitterClient:GetList( id, user )
	if not user then
		user = self.UserName
	end
	return self:MakeQuery( "GetList", { id = id, user = user } )
end

--- Deletes the specified list. Must be owned by the authenticated user.
-- @param id Required. The id or slug of the list.
-- @return table
function TwitterClient:DeleteList( id )
	return self:MakeQuery( "DeleteList", { id = id, user = self.UserName, _method = "DELETE" } )
end

--- Deletes the specified list. Must be owned by the authenticated user.
-- @param args.id Required. The id or slug of the list.
-- @param args.user Optional. ID, screen name or user ID of wanted user. Default: authenticated user.
-- @param args.since_id Optional. Returns only statuses with an ID greater than (that is, more recent than) the specified ID.
-- @param args.max_id. Optional. Returns only statuses with an ID less than (that is, older than) or equal to the specified ID.
-- @param args.per_page. Optional. Specifies the number of statuses to retrieve. May not be greater than 200.
-- @param args.page. Optional. Specifies the page of results to retrieve. Note: there are pagination limits.
-- @return table
function TwitterClient:GetListStatuses( args )
	if args and not args.user then
		args.user = self.UserName
	end
	return self:MakeQuery( "GetListStatuses", args )
end

--- List the lists the specified user has been added to.
-- @param user Optional. ID, screen name or user ID of wanted user. Default: authenticated user.
-- @param cursor Optional. Breaks the results into pages. A single page contains 20 lists. Provide a value of -1 to begin paging. Provide values as returned to in the response body's next_cursor and previous_cursor attributes to page back and forth in the list.
-- @return table
function TwitterClient:GetListMemberships( user, cursor )
	if not user then
		user = self.UserName
	end
	if not cursor then
		cursor = "-1"
	end
	return self:MakeQuery( "GetListMemberships", { user = user, cursor = cursor } )
end

--- List the lists the specified user follows.
-- @param user Optional. ID, screen name or user ID of wanted user. Default: authenticated user.
-- @param cursor Optional. Breaks the results into pages. A single page contains 20 lists. Provide a value of -1 to begin paging. Provide values as returned to in the response body's next_cursor and previous_cursor attributes to page back and forth in the list.
-- @return table
function TwitterClient:GetListSubscriptions( user, cursor )
	if not user then
		user = self.UserName
	end
	if not cursor then
		cursor = "-1"
	end
	return self:MakeQuery( "GetListSubscriptions", { user = user, cursor = cursor } )
end


-- List Members Methods
-----------------------

--- Returns the members of the specified list.
-- @param list_id. Required. The id or slug of the list
-- @param user Optional. ID, screen name or user ID of wanted user. Default: authenticated user.
-- @param cursor Optional. Breaks the results into pages. A single page contains 20 lists. Provide a value of -1 to begin paging. Provide values as returned to in the response body's next_cursor and previous_cursor attributes to page back and forth in the list.
-- @return table
function TwitterClient:GetListMembers( list_id, user, cursor )
	if not user then
		user = self.UserName
	end
	if not cursor then
		cursor = "-1"
	end
	return self:MakeQuery( "GetListMembers", { list_id = list_id, user = user, cursor = cursor } )
end

--- Add a member to a list. The authenticated user must own the list to be able to add members to it. Lists are limited to having 500 members.
-- @param id Required. The id or slug of the list
-- @param user Required. The id or screen name of the user to add as a member of the list.
-- @return table
function TwitterClient:AddUserToList( id, user )
	return self:MakeQuery( "AddUserToList", { list_id = id, id = user, user = self.UserName } )
end

--- Removes the specified member from the list. The authenticated user must be the list's owner to remove members from the list.
-- @param id Required. The id or slug of the list
-- @param user Required. The id of the member you wish to remove from the list.
-- @return table
function TwitterClient:RemoveUserFromList( id, user )
	return self:MakeQuery( "RemoveUserFromList", { list_id = id, id = user, user = self.UserName, _method = "DELETE" } )
end

--- Check if a user is a member of the specified list.
-- @param list_id Required. The id or slug of the list
-- @param user Required. ID, screen name or user ID of list's owner.
-- @param id Required. The id or screen name of the user who you want to know is a member or not of the specified list. Default: authenticated user.
-- @return table
function TwitterClient:GetListMemberID( list_id, user, id )
	if not id then
		id = self.UserName
	end
	return self:MakeQuery( "GetListMemberID", { list_id = list_id, user = user, id = id } )
end


-- List Subscribers Methods
---------------------------

--- Returns the subscribers of the specified list.
-- @param list_id Required. The id or slug of the list
-- @param user Optional. ID, screen name or user ID of wanted user. Default: authenticated user.
-- @param cursor Optional. Breaks the results into pages. A single page contains 20 lists. Provide a value of -1 to begin paging. Provide values as returned to in the response body's next_cursor and previous_cursor attributes to page back and forth in the list.
-- @return table
function TwitterClient:GetListSubscribers( list_id, user, cursor )
	if not user then
		user = self.UserName
	end
	if not cursor then
		cursor = "-1"
	end
	return self:MakeQuery( "GetListSubscribers", { list_id = list_id, user = user, cursor = cursor } )
end

--- Make the authenticated user follow the specified list.
-- @param list_id Required. The id or slug of the list
-- @param user Required. ID, screen name or user ID of list's owner.
-- @return table
function TwitterClient:SubscribeList( list_id, user )
	return self:MakeQuery( "SubscribeList", { list_id = list_id, user = user } )
end

--- Unsubscribes the authenticated user form the specified list.
-- @param list_id Required. The id or slug of the list
-- @param user Required. The ID or screen name of the list's owner
-- @param id Required. The ID or screen name of the user to remove from the list. Default: authenticated user.
-- @return table
function TwitterClient:UnsubscribeList( list_id, user, id )
	if not id then
		id = self.UserName
	end
	return self:MakeQuery( "UnsubscribeList", { list_id = list_id, user = user, id = id, _method = "DELETE" } )
end

--- Check if the specified user is a subscriber of the specified list.
-- @param list_id Required. The id or slug of the list
-- @param user Required.  The ID or screen name of the list's owner
-- @param id Required. The id of the user who you want to know is a subcriber or not of the specified list.
-- @return table
function TwitterClient:GetListSubscriberID( list_id, user, id )
	return self:MakeQuery( "GetListSubscriberID", { list_id = list_id, user = user, id = id } )
end

----------------------------------
-- Creating TwitterClient instance
----------------------------------

--- Creates TwitterClient instance
-- @param accessToken User's access token for making OAuth requests, when not provided you should obtain it and store (for example in database)
-- @param accessTokenSecret User's access token secret for signing OAuth requests, when not provided you should obtain it and store (for example in database)
-- @param appKey Your application Consumer key
-- @param appSecret Your application Consumer secret
-- @param tokenReadyUrl Where should we return to after successfully authentication? (not tested)
-- @return table TwitterClient object
function Client( accessToken, accessTokenSecret, appKey, appSecret, tokenReadyUrl )
	newClient = TwitterClient:Create()
   newClient.consumer = oauth.newConsumer({
      consumer_key = appKey,
      consumer_secret = appSecret,
      request_token_url = AuthUrls.RequestToken.url,
      authorize_url = AuthUrls.Authorize.url,
      access_token_url = AuthUrls.AccessToken.url,
      token_ready_url = tokenReadyUrl or 'oob'
   })
   if accessToken and accessTokenSecret then
      newClient:SetAccessToken( accessToken, accessTokenSecret )
   end
	return newClient
end
