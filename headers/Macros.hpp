// RFC 2812

//Numeric replies: (not all of them.. and mayce we should not use all of them...)
	#define ERR_ALREADYREGISTERED	"462"
	#define ERR_BADCHANNELKEY
	#define ERR_BADCHANMASK
	#define ERR_BANNEDFROMCHAN
	#define ERR_CANNOTSENDTOCHAN	"404"
	#define ERR_CANTKILLSERVER
	#define ERR_CHANNELISFULL
	#define ERR_CHANOPRIVSNEEDED	"482"
	#define ERR_ERRONEUSNICKNAME
	#define ERR_INVITEONLYCHAN		"473"
	#define ERR_NEEDMOREPARAMS		"461"
	#define ERR_NICKCOLLISION
	#define ERR_NICKNAMEINUSE
	#define ERR_NONICKNAMEGIVEN		"431"
	#define ERR_NOOPERHOST
	#define ERR_NOPRIVILEGES
	#define ERR_NORECIPIENT
	#define ERR_NOORIGIN
	#define ERR_NOSUCHCHANNEL		"403"
	#define ERR_NOSUCHNICK			"401"
	#define ERR_NOSUCHSERVER
	#define ERR_NOTEXTTOSEND
	#define ERR_NOTONCHANNEL		"442"
	#define ERR_NOTOPLEVEL
	#define ERR_NOTREGISTERED		"451"
	#define ERR_PASSWDMISMATCH		"464"
	#define ERR_RESTRICTED
	#define ERR_TOOMANYCHANNELS
	#define ERR_TOOMANYMATCHES
	#define ERR_TOOMANYTARGETS
	#define ERR_UMODEUNKNOWNFLAG
	#define ERR_UNAVAILRESOURCE
	#define ERR_USERDONTMATCH
	#define ERR_USERNOTINCHANNEL	"441"
	#define ERR_USERONCHANNEL		"443"
	#define ERR_WHOISCHANNELS
	#define ERR_WILDTOPLEVEL
	#define ERR_UNKNOWNCOMMAND		"421"


// Command responses

//Replies 001 to 004 sent to user upon successful registration
	#define RPL_WELCOME				"001"
	#define RPL_YOURHOST			"002"
	#define RPL_CREATED				"003"
	#define RPL_MYINFO				"004"
	
	#define RPL_AWAY
	#define RPL_CHANNELMODEIS		"324"
	#define RPL_ENDOFMOTD
	#define RPL_ENDOFNAMES			"366"
	#define RPL_ENDOFWHO			"315"
	#define RPL_ENDOFWHOIS
	#define RPL_ENDOFWHOWAS
	#define RPL_INVITING
	#define RPL_LIST
	#define RPL_LISTENED
	#define RPL_MOTD
	#define RPL_MOTDSTART
	#define RPL_NAMREPLY			"353"
	#define RPL_NOMOTD
	#define RPL_TOPIC
	#define RPL_UMODEIS				"221"
	#define RPL_WHOISCHANNELS
	#define RPL_WHOISIDLE
	#define RPL_WHOISOPERATOR
	#define RPL_WHOISSERVER
	#define RPL_WHOISUSER
	#define RPL_WHOSPCRPL			"354"
	#define RPL_WHOWASUSER
	#define RPL_WHOREPLY
	#define RPL_YOUREOPER


//Commands:
	#define PASS		// set a connection password
	#define NICK		// give or change user nickname
	#define USER 		// specify username, hostname, realname of the user
	#define OPER 		// get operator privileges
	#define MODE 		// changes the user mode: *((+ / -) *(a i w o O r s))
	#define MODE 		// change channel mode
	#define QUIT 		// a client session is terminated
	#define JOIN 		// start listening to a channel
	#define PART 		// user is removed from the list of active members for all given channels
	#define TOPIC		// change or view the topic of a channel
	#define NAMES 	// list all nicknames visible to the user
	#define LIST		// list all channels
	#define INVITE	// invite user to a channel
	#define KICK		// request force removal of a user from a channel
	#define KILL		// closes a client-server connection
	#define PING		// test the presence of an active client
	#define PONG		// reply to ping

	#define PRIVMSG // private messages between users from user to channels
	#define NOTICE	// PRIVMSG without auto reply
	#define MOTD		// get "message of the day" of the given server
	#define WHO			// returns a list of info which matches the <mask> parameter
	#define WHOIS		// query info about a particular user
	#define WHOWAS	// query info about a particular user that no longer exists

//	server to server communication (probably not applicable to the project)
//	#define SQUIT		// disconnect server links 
//	#define LUSERS	// statistics about IRC network (didn't wrote the respective replies)
//	#define VERSION	// query the version of the server
//	#define STATS		// query statistic of a server
//	#define LINKS		// list all servernames
//	#define TIME		// query local time from a specified server
//	#define CONNECT	// request a server to try to connect to other server
//	#define TRACE		// find the route to specific server 
//	#define ADMIN		// find info about administrator of given server
//	#define INFO		// get info from target server
//	#define ERROR		// server reports fatal error to its peers

//	services (not sure if applicable to the project)
//	#define SERVLIST/SQUERY	// list services connected to the network

// Optional features:
/**
 * AWAY
 * REHASH
 * DIE
 * RESTART
 * SUMMON
 * USERS
 * WALLOPS
 * USERHOST
 * ISON
 * 
*/
