#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <string>
# include <vector>
# include "Client.hpp"

class Channel
{
private:
	std::string	_name;
	std::vector<Client*> _clients;
	std::vector<Client*> _operators;
	std::string _topic;
	
public:
	explicit Channel(std::string name, Client client, Client op, std::string topic):
	_name(name),
	_topic(topic)
	{
		_clients.push_back(&client);
		_operators.push_back(&op);
	}

	~Channel(){};
};


#endif