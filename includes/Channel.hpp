#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <string>
# include <vector>
# include "Client.hpp"

class Channel
{
	
public:

	std::string				_name;
	int						_maxClients;
	int						creationTime;
	std::vector<Client>		_clients;
	std::vector<Client>		_operators;
	std::string				_topic;
	std::vector<char*>		_flags;


	explicit Channel(std::string name, Client client): _name(name)
	{
		_clients.push_back(client);
		_operators.push_back(client);
	}

	~Channel(){};

	void addClient(Client client);
	void addOperator(Client op);
	void removeClient(Client client);
	void removeOp(Client op);
	void joinChannel(Client client);
	std::string getTopic();
	void setTopic(std::string topic);
	std::string getName();

};


#endif