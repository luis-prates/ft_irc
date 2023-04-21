	#include "../includes/Channel.hpp"
	#include "../includes/Server.hpp"
	
	void Channel::addClient(Client client) {
		_clients.push_back(client);
	}

	void Channel::addOperator(Client op) {
		_operators.push_back(op);
	}

	bool	Channel::removeClient(Client client) {
	for (std::vector<Client>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
		if (it->getNickname() == client.getNickname()) {
			_clients.erase(it);
			return (true);
		}
	}
	return (false);
}

	void Channel::removeOp(Client op)	{
		for (std::vector<Client>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
			if (it->getNickname() == op.getNickname()) {
				_clients.erase(it);
				break;
			}
			{
				_clients.erase(it);
				break;
			}
		}
	}

	void Channel::joinChannel(Client client) {
		Channel::addClient(client);
		Channel::addOperator(client);
	}

	std::string Channel::getTopic() { return (_topic); }

	void Channel::setTopic(std::string topic) { _topic = topic; }

	std::string Channel::getName() { return (_name); }

int Server::joinChannel( std::vector<std::string> params, Client &client, std::string &response) {
/** @checks:
 * User must be invited if the channel is +i;
 * The user's nick/username/hostname must not match any active bans;
 * The correct password must be given if the channel is +k;
 * 
 * @reply:
 * Receive notice about all commands their server receives that affect the channel:
 * -	MODE	-	KICK	-	PART	-	QUIT	-	PRIVMSG	-	NOTICE
 * 
 * If a JOIN is successful, the user is then sent the channel's topic (using RPL_TOPIC)
 * and the list of users who are on the channel (using RPL_NAMREPLY),
 * which MUST include the user joining.
 * 
 */	
		// check if the channel name is valid

	if (params.size() != 1)
	{
		response = "Error: channel name cannot contain spaces\r\n";
		send(client.getSocketFd(), response.c_str(), response.size(), 0);
		return (0);
	}

	// check if the client is registered
	if (!client.isRegistered())
	{
		response = "Error: you must be registered to join a channel\r\n";
		send(client.getSocketFd(), response.c_str(), response.size(), 0);
		return (0);
	}

	// TODO: Check if channel name has comma
	if (params[0].find(',') != std::string::npos)
	{
		response = "Error: channel name cannot contain commas\r\n";
		send(client.getSocketFd(), response.c_str(), response.size(), 0);
		return (0);
	}

	//TODO: Check if channel name has a control G/BEL
	if (params[0].find('\a') != std::string::npos)
	{
		response = "Error: channel name cannot contain control G/BEL\r\n";
		send(client.getSocketFd(), response.c_str(), response.size(), 0);
		return (0);
	}

	// create a channel object and add it to the list of channels
	std::string channelName = params[0];

/** 
*	if channel name is valid check if the channel already exists
*	if not, create a new channel and send a reply to the JOIN command
*/
	if (params[0][0] == '#') {
		if (!checkChannel(channelName, client))
			createNewChannel(channelName, client, response);
		rpl_Join(client, *Server::getChannel(channelName), response);
	}
	else  {
		response = channelName + ":No such channel\r\n";
		if (send(client.getSocketFd(), response.c_str(), response.size(), 0) == -1)
			std::cout << "error sending response\n";
	}
	return (0);
}

// Create a new channel
void Server::createNewChannel(std::string channelName, Client &client, std::string response) {
	// Create a new Channel, add it to the list of channels and add the client to the channel
	Channel newChannel(channelName, client);
	Server::_channels.push_back(newChannel);
	std::cout << "Channel " << newChannel.getName() << " created\n";
	newChannel.joinChannel(client); // define the client and operator
	std::cout << "Client " << client.getNickname() << " joined channel " << newChannel.getName() << "\n";
	client.addChannel(channelName);
}

// Check if channel already exists
int	Server::checkChannel(std::string channelName, Client &client){
	std::vector<Channel>::iterator it;
	for(it = Server::_channels.begin(); it != Server::_channels.end(); ++it) {
		if (it->_name == channelName)	{
			it->addClient(client);
			client.addChannel(channelName);
			return (1);
		}
	}
	return (0);
}

bool	Channel::isClientInChannel(Client client) {
	for (std::vector<Client>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
		if (it->getNickname() == client.getNickname())
			return (true);
	}
	return (false);
}

/* REFERENCES:
	* https://se.inf.ethz.ch/student_projects/fabian_gremper/Report.pdf
	*/