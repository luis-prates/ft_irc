#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <string>
# include <vector>
# include "Client.hpp"

class Channel
{
	
public:

	std::string								_name;
	int												_maxClients;
	int												creationTime;
	std::vector<Client>				_clients;
	std::vector<std::string>	_operators;
	std::string								_topic;
	std::vector<char>					_mode;


	explicit Channel(std::string name, Client client): _name(name)
	{
		_clients.push_back(client);
		_operators.push_back(client.getNickname());
		_topic = "No topic is set";
		_mode.push_back('n');
		_mode.push_back('t');
	}

	~Channel(){};

	void addClient(Client client);
	void addOperator(Client op);
	void removeClient(Client client);
	void removeOp(Client op);
	void joinChannel(Client client);
	std::string getTopic();
	void setTopic(std::string topic, Client client);
	std::string getName();
	std::string getMode();

};


#endif