#include <string>
#include <map>
#include <vector>



using namespace std;

class server
{
	string name; // Max len 63
};

class client
{
	// For each client, all servers MUST have the following info:
	string unique_id;
	string server_connected;
};

class user 
{
	// Users
	string unique_nickname; // Max len 9

	// For each user, all servers MUST have the following info:
	string host_name;
	string username_on_host;
	string server_connected;
};

/* Services and users don't have the same type of access to the server */
class service
{
	public:
		map<string, string> service_name; // nickname(9), server_name
		string service_type;
	};

	class channel
	{
		vector<string> channel_members;
		string channel_mode;
};
