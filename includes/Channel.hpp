#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <string>
# include <vector>
# include "Client.hpp"

class Channel
{
	
public:

	std::string					_name;
	int							_maxClients;
	int							creationTime;
	std::vector<Client>			_clients;
	std::vector<Client>			_operators;
	std::vector<std::string>	_invited;
	std::string					_topic;
	std::vector<char>			_mode;


	explicit Channel(std::string name, Client client): _name(name)
	{
		_operators.push_back(client);
		_topic = "";
		_mode.push_back('n');
		_mode.push_back('t');
	}

	~Channel(){};

	void							addClient(Client client);
	void							addOperator(Client op);
	bool							removeClient(Client client);
	bool							removeOp(Client op);
	std::string						getTopic();
	void							setTopic(std::string topic);
	std::string						getName();
	std::string						getMode();
	std::vector<char>				getModeVector();
	void							addMode(char mode);
	void							removeMode(char mode);
	bool							isClientInChannel(Client client);
	bool							isClientInChannel(std::string nickname);
	bool							isOperatorInChannel(Client client);
	bool							isOperatorInChannel(std::string nickname);
	bool							isModeSet(char mode);
	bool							isInvited(Client client);
	bool							isInvited(std::string nickname);
	void							addInvited(Client client);
	void							addInvited(std::string nickname);
	void							removeInvited(Client client);
	void							removeInvited(std::string nickname);
	std::vector<Client>::iterator	findClient(Client client);
	std::vector<Client>::iterator	findClient(std::string nickname);
	std::vector<Client>::iterator	findOperator(Client client);
	std::vector<Client>::iterator	findOperator(std::string nickname);
};


#endif