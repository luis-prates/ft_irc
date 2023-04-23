#include "../includes/Server.hpp"
#include "../includes/Channel.hpp"
#include "../headers/Macros.hpp"

Server::Server(std::string password): _password(password)
{
	
}

Server::~Server()
{
	
}

int	Server::setup_server(int port)
{
	int	sockOpt = 1;

	_addrlen = sizeof(_address);
	// create the socket
	if ((_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0)
	{
		std::cerr << "Error: Failed to create the server socket\n";
		perror("socket failed");
		throw ServerException();
	}

	// Set socket options
	if (setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, &sockOpt, sizeof(int)) < 0)
	{
		std::cerr << "Error: Failed to set socket options\n";
		perror("setsockopt");
		throw ServerException();
	}
	// Set the socket to non-blocking mode
	if (fcntl(_socket, F_SETFL, O_NONBLOCK) == -1) {
		std::cerr << "Error setting socket flags: " << strerror(errno) << std::endl;
		throw ServerException();
	}

	_address.sin_family = AF_INET;
	// INADDR_ANY is a special IP address that tells the socket to listen on all available network interfaces.
	_address.sin_addr.s_addr = INADDR_ANY;
	_address.sin_port = htons(port);

	if (bind(_socket, (struct sockaddr *)&_address, _addrlen) < 0)
	{
		std::cerr << "Error: Failed to bind the server socket to a port\n";
		perror("bind failed");
		throw ServerException();
	}
	if (listen(_socket, MAX_CONNECTIONS) < 0)
	{
		std::cerr << "Error: Failed to start listening for incoming connections\n";
		perror("listen");
		throw ServerException();
	}
	std::cout << "Server started and listening for incoming connections on port " << port << std::endl;

	return (EXIT_SUCCESS);
}

bool	Server::run()
{
	fdResetNSet(_clients);

	if (select(_maxFd + 1, &_readFds, NULL, NULL, NULL) < 0)
		return (false);
	//If something happened on the master socket,
	//then its an incoming connection
	if (FD_ISSET(this->getSocket(), &_readFds))
		if (handleNewConnection(_clients) == EXIT_FAILURE)
			return (false);

	//else it's some IO operation on some other socket
	for (int i = 0; i < MAX_CLIENTS; i++)
	{
		if (FD_ISSET(_clients[i].getSocketFd(), &_readFds))
		{
			//Check if it was for closing, and also read the
			//incoming message
			if (handleClientInput(_clients[i]) == EXIT_FAILURE) {
				return (true);
			}
		}
	}
	return (true);
}

int	Server::clear_fd_set()
{
	// Clean up
	for (int fd = 0; fd <= _maxFd; fd++)
		if (FD_ISSET(fd, &_readFds))
			close(fd);
	close(this->getSocket());
	return (EXIT_SUCCESS);
}


int	Server::fdResetNSet(std::vector<Client> &clients)
{
	int	clientFd;

	// clear the socket set
	FD_ZERO(&_readFds);
	// add server socket to set
	FD_SET(this->getSocket(), &_readFds);
	_maxFd = this->getSocket();

	//add child sockets to set 
	for (int i = 0; i < MAX_CLIENTS ; i++)
	{
		//socket descriptor 
		clientFd = clients[i].getSocketFd();

		//if valid socket descriptor then add to read list
		if(clientFd > 0)
			FD_SET(clientFd, &_readFds);
				
		//highest file descriptor number, need it for the select function
		if(clientFd > _maxFd)
			_maxFd = clientFd;
	}
	return (EXIT_SUCCESS);
}

int	Server::handleNewConnection(std::vector<Client> &clients)
{
	t_socket	tmpSocket;

	tmpSocket.addrlen = sizeof(tmpSocket.address);
	if ((tmpSocket.socket = accept(this->getSocket(),
			(struct sockaddr *)&tmpSocket.address, (socklen_t*)&tmpSocket.addrlen)) < 0)
	{
		perror("accept");
		return (EXIT_FAILURE);
	}
		
	//inform user of socket number - used in send and receive commands
	std::cout << "New connection, socket fd is " << tmpSocket.socket << \
			", ip is: " << inet_ntoa(tmpSocket.address.sin_addr) << \
			", port: " << ntohs(tmpSocket.address.sin_port) << std::endl;

	//add new socket to array of sockets
	for (int i = 0; i < MAX_CLIENTS; i++)
	{
		//if position is empty 
		if( clients[i].getSocketFd() == 0 )
		{
			clients[i].setSocketFd(tmpSocket.socket);
			clients[i].setIpAddress(inet_ntoa(tmpSocket.address.sin_addr));
			clients[i].setPort(ntohs(tmpSocket.address.sin_port));
			break;
		}
	}
	return (EXIT_SUCCESS);
}

int	Server::handleClientInput(Client &client)
{
	int		valread;
	char	buffer[BUFFER_SIZE];
	int		dcReturn = 1;

	//Check if it was for closing , and also read the 
	//incoming message
	if ((valread = recv(client.getSocketFd(), buffer, BUFFER_SIZE - 1, 0)) <= 0)
	{
		std::cout << "Host disconnected, ip: " << client.getIpAddress() << \
				" port:" << client.getPort() << std::endl;
		if (client.isRegistered()) {
			std::vector<std::string> dcParams;
			std::string dcMsg("Client disconnected from server");
			dcParams.push_back(dcMsg);
			dcReturn = quit(dcParams, client);
		}
		if (dcReturn)
			cleanClientFromServer(client);
		return (EXIT_FAILURE);
	}
	//Echo back the message that came in 
	else 
	{
		//set the string terminating NULL byte on the end 
		//of the data read
		buffer[valread] = '\0';
		// Handle the incoming message
		client.getOutputBuffer().append(buffer, valread);
		// '\012' represents \r\n
		if (client.getOutputBuffer().find('\012') != std::string::npos)
		{
			std::vector<std::string> commands = split(client.getOutputBuffer(), '\012');
			for (size_t k = 0; k < commands.size(); k++)
			{
				commands[k].erase(std::find(commands[k].begin(), commands[k].end(), '\r'), commands[k].end());
				if (handleCommands(commands[k], client) == 2)
					return (EXIT_FAILURE);
			}
			client.getOutputBuffer().clear();
		}
	}
	return (EXIT_SUCCESS);
}

// needs refactoring and proper handling of commands
// used for testing with HexChat client
int	Server::handleCommands(std::string message, Client &client)
{
	std::string								command;
	std::vector<std::string>				params;
	std::string								response;
	std::map<std::string, commandHandler>	commandMap;

	commandMap["nick"] = &Server::handleNick;
	commandMap["user"] = &Server::user;
	commandMap["join"] = &Server::joinChannel;
	commandMap["who"] = &Server::who;
	commandMap["privmsg"] = &Server::privmsg;
	commandMap["notice"] = &Server::notice;
	commandMap["part"] = &Server::part;
	commandMap["quit"] = &Server::quit;
	commandMap["pass"] = &Server::pass;
	commandMap["mode"] = &Server::mode;
	commandMap["invite"] = &Server::invite;
	commandMap["topic"] = &Server::topic;
	commandMap["kick"] = &Server::kick;

	// check if the message is a command
	// get the command
	command = message.substr(0, message.find(' '));
	std::transform(command.begin(), command.end(), command.begin(), ::tolower);
	// get the parameters
	params = split(message.substr(message.find(' ') + 1), ' ');
	// check if the command is valid
	std::map<std::string, commandHandler>::iterator it = commandMap.find(command);
	if (it != commandMap.end())
	{
		// call the command handler
		// this is a pointer to the function in the map
		(this->*(it->second))(params, client);
	}
	else
		return (invalidCommand(command, client));
	return (EXIT_SUCCESS);
}

std::vector<std::string>	Server::split(std::string message, char del)
{
	// variable to store token obtained from the original
	// string
	std::string s;

	// constructing stream from the string
	std::stringstream ss(message);

	// declaring vector to store the string after split
	std::vector<std::string> v;

	// using while loop until the getline condition is
	// satisfied
	// ' ' represent split the string whenever a space is
	// found in the original string
	while (getline(ss, s, del))
		v.push_back(s);
	return (v);
}

int	Server::handleNick(std::vector<std::string> params, Client &client)
{
	std::string	response;
	std::string	prevNickname;

	// check if the nickname is valid
	// The syntax for this command is "NICK <nickname>". For example, "NICK John"
	if (params.size() != 1)
	{
		response = "Error: nickname cannot contain spaces\r\n";
		sendMessage(client.getSocketFd(), response);
		return (EXIT_FAILURE);
	}
	prevNickname = client.getNickname();
	client.setNickname(params[0]);
	if (prevNickname.empty())
		response = ":" + client.getNickname() + "!" + client.getUsername() + "@" + client.getIpAddress() + " NICK :" + params[0] + "\r\n";
	else
		response = ":" + prevNickname + "!" + client.getUsername() + "@" + client.getIpAddress() + " NICK :" + params[0] + "\r\n";
	if (sendMessage(client.getSocketFd(), response) == -1)
			return (EXIT_FAILURE);
	return (EXIT_SUCCESS);
}



// Reply to the client to confirm the join
int	Server::rpl_Join(Client client, Channel &newChannel) {
	std::string	response;

	response = ":" + client.getNickname() + " JOIN " + newChannel.getName() + "\r\n";
	for (std::vector<Client>::iterator itClient = newChannel._clients.begin(); itClient != newChannel._clients.end(); ++itClient) {
		if (sendMessage(itClient->getSocketFd(), response) == -1)
			return (EXIT_FAILURE);
	}
	for (std::vector<Client>::iterator itOperator = newChannel._operators.begin(); itOperator != newChannel._operators.end(); ++itOperator) {
		if (sendMessage(itOperator->getSocketFd(), response) == -1)
			return (EXIT_FAILURE);
	}
	// RPL_TOPIC
	if (newChannel.getTopic() != "")
		response = ":" + this->getHostname() + " " + RPL_TOPIC + " " + client.getNickname() + " " + newChannel.getName() + " " + newChannel.getTopic() + "\r\n";
	else
		response = ":" + this->getHostname() + " " + RPL_NOTOPIC + " " + client.getNickname() + " " + newChannel.getName() + " :No topic is set\r\n";
	if (sendMessage(client.getSocketFd(), response) == -1)
		return (EXIT_FAILURE);
	return (EXIT_SUCCESS);
}

std::vector<Channel>::iterator	Server::getChannel(std::string channelName) {
	for(std::vector<Channel>::iterator it = _channels.begin(); it != _channels.end(); ++it) {
		if (it->_name == channelName)	{
			return (it);
		}
	}
	return (_channels.end());
}

int	Server::who(std::vector<std::string> params, Client &client) {
	std::string responseNames;

	for(std::vector<Channel>::iterator itChannel = _channels.begin(); itChannel != _channels.end(); ++itChannel) {
		if (itChannel->_name == params[0])	{
			responseNames = ":" + this->getHostname() + " " + RPL_NAMREPLY + " " + client.getNickname() + " = " + itChannel->getName() + " :";
			for (std::vector<Client>::iterator itOperator = itChannel->_operators.begin(); itOperator != itChannel->_operators.end(); ++itOperator) {
				// :hostname 353 nickname = #channel :nickname nickname (can be more than one here or sent in multiple messages)
				responseNames += "@" + itOperator->getNickname() + " ";
			}
			for (std::vector<Client>::iterator itClient = itChannel->_clients.begin(); itClient != itChannel->_clients.end(); ++itClient) {
				// :hostname 353 nickname = #channel :nickname nickname (can be more than one here or sent in multiple messages)
				responseNames += itClient->getNickname() + " ";
			}
			responseNames += "\r\n:" + this->getHostname() + " " + RPL_ENDOFNAMES + " " + client.getNickname() + " " + itChannel->getName() + " :End of /NAMES list.\r\n";
			if (sendMessage(client.getSocketFd(), responseNames) == -1)
				return (EXIT_FAILURE);
		}
	}
	return (EXIT_SUCCESS);
}

int	Server::privmsg(std::vector<std::string> params, Client &client) {
	// check if the channel exists
	std::string response;
	std::string msg;
	std::vector<Channel>::iterator itChannel;
	std::vector<Client>::iterator itClient;

	if (params.size() < 2)
	{
		response = ":" + this->getHostname() + " " + "412" + " " + client.getNickname() + " :No text to send\r\n";
		if (sendMessage(client.getSocketFd(), response) == -1)
			return (EXIT_FAILURE);
		return (EXIT_SUCCESS);
	}

	if (params[0].at(0) == '#') {
		for(itChannel = _channels.begin(); itChannel != _channels.end(); ++itChannel) {
			if (itChannel->_name == params[0])	{
				if (!itChannel->isClientInChannel(client) && !itChannel->isOperatorInChannel(client))
				{
					// :hostname 404 nickname #channel :Cannot send to channel
					response = ":" + this->getHostname() + " " + ERR_CANNOTSENDTOCHAN + " " + client.getNickname() + " " + params[0] + " :Cannot send to channel\r\n";
					if (sendMessage(client.getSocketFd(), response) == -1)
						return (EXIT_FAILURE);
					return (EXIT_SUCCESS);
				}
				// message to channel
				for (size_t i = 1; i < params.size(); i++)
						msg += params[i] + " ";
				if (msg.empty())
					msg.erase(msg.size() - 1);
				// Reply to the client to send message to channel
				response = ":" + client.getNickname() + " PRIVMSG " + itChannel->getName() + " " + msg + "\r\n";
				for (itClient = itChannel->_clients.begin(); itClient != itChannel->_clients.end(); ++itClient) {
					// Don't send the response to the sender
					if (client.getNickname() != itClient->getNickname())
						if (sendMessage(itClient->getSocketFd(), response) == -1)
							return (EXIT_FAILURE);
				}
				for (itClient = itChannel->_operators.begin(); itClient != itChannel->_operators.end(); ++itClient) {
					// Don't send the response to the sender
					if (client.getNickname() != itClient->getNickname())
						if (sendMessage(itClient->getSocketFd(), response) == -1)
							return (EXIT_FAILURE);
				}
				return (EXIT_SUCCESS);
			}
		}
		if (itChannel == _channels.end()) {
			response = ":" + this->getHostname() + " " + ERR_NOSUCHCHANNEL + " " + client.getNickname() + " " + params[0] + " :No such channel\r\n";
			if (sendMessage(client.getSocketFd(), response) == -1)
				return (EXIT_FAILURE);
			return (EXIT_SUCCESS);
		}
	}
	else {
		// message to user
		for (itClient = _clients.begin(); itClient != _clients.end(); ++itClient) {
			if (itClient->getNickname() == params[0]) {
				for (size_t i = 1; i < params.size(); i++)
					msg += params[i] + " ";
				// Reply to the client to send message to user
				response = ":" + client.getNickname() + " PRIVMSG " + itClient->getNickname() + " " + msg + "\r\n";
				if (sendMessage(itClient->getSocketFd(), response) == -1)
					return (EXIT_FAILURE);
				return (EXIT_SUCCESS);
			}
		}
		if (itClient == _clients.end()) {
			response = ":" + this->getHostname() + " " + ERR_NOSUCHNICK + " " + client.getNickname() + " " + params[0] + " :No such nick\r\n";
			if (sendMessage(client.getSocketFd(), response) == -1)
				return (EXIT_FAILURE);
		}
		return (EXIT_SUCCESS);
	}
	return (EXIT_SUCCESS);
}

int	Server::notice(std::vector<std::string> params, Client &client)
{
	std::string response;
	std::vector<Client>::iterator itClient;
	std::vector<Channel>::iterator itChannel;
	
	if (params.size() < 2) {
		if (client.isRegistered())
			response = ":" + this->getHostname() + " " + ERR_NEEDMOREPARAMS + " " + client.getNickname() + " NOTICE :Not enough parameters\r\n";
		else
			response = ":" + this->getHostname() + " " + ERR_NEEDMOREPARAMS + " * NOTICE :Not enough parameters\r\n";
	}
	if (client.isRegistered()) {
		if (params[0].at(0) == '#') {
			// message to channel
			itChannel = getChannelIterator(params[0]);
			if (itChannel != _channels.end() && (itChannel->isClientInChannel(client) || itChannel->isOperatorInChannel(client))) {
				for (size_t i = 1; i < params.size(); i++)
					response += params[i] + " ";
				if (response.empty())
					response.erase(response.size() - 1);
				response = ":" + client.getNickname() + "!" + client.getUsername() + "@" + client.getNickname() + " NOTICE " + params[0] + " " + response + "\r\n";
				for (itClient = itChannel->_clients.begin(); itClient != itChannel->_clients.end(); ++itClient) {
					// Don't send the response to the sender
					if (client.getNickname() != itClient->getNickname())
						if (sendMessage(itClient->getSocketFd(), response) == -1)
							return (EXIT_FAILURE);
				}
				for (itClient = itChannel->_operators.begin(); itClient != itChannel->_operators.end(); ++itClient) {
					// Don't send the response to the sender
					if (client.getNickname() != itClient->getNickname())
						if (sendMessage(itClient->getSocketFd(), response) == -1)
							return (EXIT_FAILURE);
				}
				return (EXIT_SUCCESS);
			}
		}
		else {
			// message to user
			itClient = getClientIterator(params[0]);
			if (itClient != _clients.end()) {
				for (size_t i = 1; i < params.size(); i++)
					response += params[i] + " ";
				if (response.empty())
					response.erase(response.size() - 1);
				response = ":" + client.getNickname() + "!" + client.getUsername() + "@" + client.getNickname() + " NOTICE " + params[0] + " " + response + "\r\n";
				if (sendMessage(itClient->getSocketFd(), response) == -1)
					return (EXIT_FAILURE);
				return (EXIT_SUCCESS);
			}
		}
	}
	else {
		if (client.getNickname().empty())
			response = ":" + this->getHostname() + " " + ERR_NOTREGISTERED + " * :You have not registered\r\n";
		else
			response = ":" + this->getHostname() + " " + ERR_NOTREGISTERED + " " + client.getNickname() + " :You have not registered\r\n";
	}
	if (sendMessage(client.getSocketFd(), response) == -1)
		return (EXIT_FAILURE);
	return (EXIT_SUCCESS);
}

int	Server::part(std::vector<std::string> params, Client &client) {
	std::string						response;
	std::string						msg;
	std::vector<Client>::iterator	itClient;

	for(std::vector<Channel>::iterator itChannel = _channels.begin(); itChannel != _channels.end(); ++itChannel) {
		if (itChannel->_name == params[0])	{

			// remove the client from the channel
			for (itClient = itChannel->_clients.begin(); itClient != itChannel->_clients.end(); ++itClient)
				if (client.getNickname() == itClient->getNickname())
					break;

			if (itClient == itChannel->_clients.end()) {
				for (itClient = itChannel->_operators.begin(); itClient != itChannel->_operators.end(); ++itClient)
					if (client.getNickname() == itClient->getNickname())
						break;
			}
			if (itClient == itChannel->_operators.end())
				response = ":" + this->getHostname() + " " + ERR_NOTONCHANNEL + " " + itClient->getNickname() + " " + itChannel->getName() + " :You're not on that channel\r\n";
			else
				// Reply to the client to confirm the part
				response = ":" + client.getNickname() + "!" + client.getUsername() + "@" + client.getIpAddress() + " PART " + itChannel->getName() + "\r\n";
			if (sendMessage(client.getSocketFd(), response) == -1)
				return (EXIT_FAILURE);
			if (response.find("PART") != std::string::npos) {
				for (size_t i = 1; i < params.size(); i++)
					msg += params[i] + " ";
				if (!msg.empty())
					msg.erase(msg.size() - 1);
				msg = ":" + client.getNickname() + "!" + client.getNickname() + "@" + client.getIpAddress() + " PART " + itChannel->getName() + " :" + msg + "\r\n";
				if (itChannel->isClientInChannel(client))
					itChannel->_clients.erase(itClient);
				else
					itChannel->_operators.erase(itClient);
				if (itChannel->_clients.empty() && itChannel->_operators.empty())
					_channels.erase(itChannel);
				else {
					// Send the part message to the other clients in the channel
					for (itClient = itChannel->_clients.begin(); itClient != itChannel->_clients.end(); ++itClient) {
						if (sendMessage(itClient->getSocketFd(), msg) == -1)
							return (EXIT_FAILURE);
					}
					for (itClient = itChannel->_operators.begin(); itClient != itChannel->_operators.end(); ++itClient) {
						if (sendMessage(itClient->getSocketFd(), msg) == -1)
							return (EXIT_FAILURE);
					}
				}
			}
			return (EXIT_SUCCESS);
		}
	}
	response = ":" + this->getHostname() + " " + ERR_NOSUCHCHANNEL + " " + client.getNickname() + " " + params[0] + " :No such channel\r\n";
	if (sendMessage(client.getSocketFd(), response) == -1)
		return (EXIT_FAILURE);
	return (EXIT_SUCCESS);
}

int	Server::user(std::vector<std::string> params, Client &client) {
	std::string	realName;
	std::string	response;

	if (client.getNickname().empty()) {
		response = ":" + this->getHostname() + " " + ERR_NONICKNAMEGIVEN + " * :No nickname given\r\n";
		if (sendMessage(client.getSocketFd(), response) == -1)
			return (EXIT_FAILURE);
	}
	else if (params.size() < 4) {
		response = ":" + this->getHostname() + " " + ERR_NEEDMOREPARAMS + " " + client.getNickname() + " USER :Not enough parameters\r\n";
		if (sendMessage(client.getSocketFd(), response) == -1)
			return (EXIT_FAILURE);
	}
	else if (!client.isRegistered()) {
		client.setUsername(params[0]);
		for (size_t i = 3; i < params.size(); i++)
			realName += params[i] + " ";
		if (!realName.empty())
			realName.erase(realName.size() - 1);
		client.setRealname(realName);
		client.setRegistered(true);
		this->rpl_Welcome(client);
	}
	else {
		response = ":" + this->getHostname() + " " + ERR_ALREADYREGISTERED + " " + client.getNickname() + " :You may not reregister\r\n";
		if (sendMessage(client.getSocketFd(), response) == -1)
			return (EXIT_FAILURE);
	}
	return (EXIT_SUCCESS);
}

int	Server::pass(std::vector<std::string> params, Client &client)
{
	std::string	response;

	// check if the client is registered
	if (client.isRegistered())
	{
		response = ":" + this->getHostname() + " " + ERR_ALREADYREGISTERED + " " + client.getNickname() + " :You may not reregister\r\n";
		if (sendMessage(client.getSocketFd(), response) == -1)
			return (EXIT_FAILURE);
		return (EXIT_SUCCESS);
	}
	// check if the password is correct
	else if (params[0] != _password)
	{
		// asterisk is used to replace any nickname
		response = ":" + this->getHostname() + " " + ERR_PASSWDMISMATCH + " * " + " :Incorrect password\r\n";
		if (sendMessage(client.getSocketFd(), response) == -1)
			return (EXIT_FAILURE);
		close(client.getSocketFd());
		FD_CLR(client.getSocketFd(), &_readFds);
		client.clearClient();
		return (2);
	}
	else
		return (EXIT_SUCCESS);
}

int Server::mode(std::vector<std::string> params, Client &client)
{
	if (!client.isRegistered())
		return (handleNotRegistered(client));
	if (params.size() == 0)
	{
		std::string	response = ":" + this->getHostname() + " " + ERR_NEEDMOREPARAMS + " " + client.getNickname() + " MODE :Not enough parameters\r\n";
		if (sendMessage(client.getSocketFd(), response) == -1)
			return (EXIT_FAILURE);
		return (EXIT_SUCCESS);
	}
	else if (params.size() == 1)
	{
		std::string	response = ":" + this->getHostname() + " " + ERR_NEEDMOREPARAMS + " " + client.getNickname() + " MODE :Channel and User modes out of this server's scope\r\n";
		if (sendMessage(client.getSocketFd(), response) == -1)
			return (EXIT_FAILURE);
		return (EXIT_SUCCESS);
	}
	else
	{
		return (modeUser(params, client));
	}
	return (EXIT_SUCCESS);
}

int Server::invite(std::vector<std::string> params, Client &client)
{
	std::string	response;

	if (!client.isRegistered())
		return (handleNotRegistered(client));
	if (params.size() < 2)
	{
		response = ":" + this->getHostname() + " " + ERR_NEEDMOREPARAMS + " " + client.getNickname() + " INVITE :Not enough parameters\r\n";
		if (sendMessage(client.getSocketFd(), response) == -1)
			return (EXIT_FAILURE);
		return (EXIT_SUCCESS);
	}
	std::string	channelName = params[1];
	std::string	nickname = params[0];
	std::vector<Channel>::iterator	itChannel;

	itChannel = this->getChannelIterator(channelName);
	if (itChannel == _channels.end())
	{
		response = ":" + this->getHostname() + " " + ERR_NOSUCHCHANNEL + " " + client.getNickname() + " " + channelName + " :No such channel\r\n";
		if (sendMessage(client.getSocketFd(), response) == -1)
			return (EXIT_FAILURE);
		return (EXIT_SUCCESS);
	}
	if (itChannel->isModeSet('i'))
	{
		if (!itChannel->isOperatorInChannel(client.getNickname()))
		{
			response = ":" + this->getHostname() + " " + ERR_CHANOPRIVSNEEDED + " " + client.getNickname() + " " + channelName + " :You're not channel operator\r\n";
			if (sendMessage(client.getSocketFd(), response) == -1)
				return (EXIT_FAILURE);
			return (EXIT_SUCCESS);
		}
		else if (!itChannel->isClientInChannel(nickname))
		{
			std::vector<Client>::iterator	itClient;
			itClient = this->getClientIterator(nickname);
			if (itClient == _clients.end())
			{
				response = ":" + this->getHostname() + " " + ERR_NOSUCHNICK + " " + client.getNickname() + " " + nickname + " :No such nickname\r\n";
				if (sendMessage(client.getSocketFd(), response) == -1)
					return (EXIT_FAILURE);
				return (EXIT_SUCCESS);
			}
			else
			{
				response = ":" + client.getNickname() + " INVITE " + nickname + " " + channelName + "\r\n";
				itChannel->addInvited(nickname);
				if (sendMessage(itClient->getSocketFd(), response) == -1)
					return (EXIT_FAILURE);
				return (EXIT_SUCCESS);
			}
		}
		else
		{
			response = ":" + this->getHostname() + " " + ERR_USERONCHANNEL + " " + client.getNickname() + " " + nickname + " " + channelName + " :is already on channel\r\n";
			if (sendMessage(client.getSocketFd(), response) == -1)
				return (EXIT_FAILURE);
			return (EXIT_SUCCESS);
		}
	}
	else if (!itChannel->isModeSet('i'))
	{
		if (!itChannel->isClientInChannel(nickname) && !itChannel->isOperatorInChannel(nickname))
		{
			std::vector<Client>::iterator	itClient;
			itClient = this->getClientIterator(nickname);
			if (itClient == _clients.end())
			{
				response = ":" + this->getHostname() + " " + ERR_NOSUCHNICK + " " + client.getNickname() + " " + nickname + " :No such nickname\r\n";
				if (sendMessage(client.getSocketFd(), response) == -1)
					return (EXIT_FAILURE);
				return (EXIT_SUCCESS);
			}
			else
			{
				response = ":" + client.getNickname() + " INVITE " + nickname + " " + channelName + "\r\n";
				if (sendMessage(itClient->getSocketFd(), response) == -1)
					return (EXIT_FAILURE);
				return (EXIT_SUCCESS);
			}
		}
		else
		{
			response = ":" + this->getHostname() + " " + ERR_USERONCHANNEL + " " + client.getNickname() + " " + nickname + " " + channelName + " :is already on channel\r\n";
			if (sendMessage(client.getSocketFd(), response) == -1)
				return (EXIT_FAILURE);
			return (EXIT_SUCCESS);
		}
	}
	return (EXIT_SUCCESS);
}

int Server::topic(std::vector<std::string> params, Client &client)
{
	std::string	response;

	if (params.size() == 0) {
		response = ":" + this->getHostname() + " " + ERR_NEEDMOREPARAMS + " " + client.getNickname() + " TOPIC :Not enough parameters\r\n";
		if (sendMessage(client.getSocketFd(), response) == -1)
			return (EXIT_FAILURE);
		return (EXIT_SUCCESS);
	}
	else if (params.size() == 1) {
		std::vector<Channel>::iterator itChannel = this->getChannelIterator(params[0]);
		if (itChannel != _channels.end()) {
			if (itChannel->getTopic().empty()) {
				response = ":" + this->getHostname() + " " + RPL_NOTOPIC + " " + client.getNickname() + " " + params[0] + " :No topic is set\r\n";
				if (sendMessage(client.getSocketFd(), response) == -1)
					return (EXIT_FAILURE);
				return (EXIT_SUCCESS);
			}
			else {
				response = ":" + this->getHostname() + " " + RPL_TOPIC + " " + client.getNickname() + " " + params[0] + " " + itChannel->getTopic() + "\r\n";
				if (sendMessage(client.getSocketFd(), response) == -1)
					return (EXIT_FAILURE);
				return (EXIT_SUCCESS);
			}
		}
		else {
			response = ":" + this->getHostname() + " " + ERR_NOSUCHCHANNEL + " " + client.getNickname() + " " + params[0] + " :No such channel\r\n";
			if (sendMessage(client.getSocketFd(), response) == -1)
				return (EXIT_FAILURE);
			return (EXIT_SUCCESS);
		}
	}
	else {
		std::vector<Channel>::iterator	itChannel = this->getChannelIterator(params[0]);
		std::string						topic;
		if (itChannel != _channels.end()) {
			if (itChannel->isOperatorInChannel(client.getNickname())) {
				for	(size_t i = 1; i < params.size(); ++i)
					topic += params[i] + " ";
				if (!topic.empty())
					itChannel->setTopic(topic.substr(0, topic.size() - 1));
				response = ":" + client.getNickname() + "!" + client.getUsername() + client.getIpAddress() + " TOPIC " + params[0] + " " + topic + "\r\n";
				for (std::vector<Client>::iterator itClient = itChannel->_clients.begin(); itClient != itChannel->_clients.end(); ++itClient) {
					if (itChannel->isClientInChannel(itClient->getNickname()))
						if (sendMessage(itClient->getSocketFd(), response) == -1)
							return (EXIT_FAILURE);
				}
				for (std::vector<Client>::iterator itOperator = itChannel->_operators.begin(); itOperator != itChannel->_operators.end(); ++itOperator) {
					if (itChannel->isOperatorInChannel(itOperator->getNickname()))
						if (sendMessage(itOperator->getSocketFd(), response) == -1)
							return (EXIT_FAILURE);
				}
				return (EXIT_SUCCESS);
			}
			else if (itChannel->isClientInChannel(client.getNickname())) {
				response = ":" + this->getHostname() + " " + ERR_CHANOPRIVSNEEDED + " " + client.getNickname() + " " + params[0] + " :You're not channel operator\r\n";
				if (sendMessage(client.getSocketFd(), response) == -1)
					return (EXIT_FAILURE);
				return (EXIT_SUCCESS);
			}
			else {
				response = ":" + this->getHostname() + " " + ERR_NOTONCHANNEL + " " + client.getNickname() + " " + params[0] + " :You're not on that channel\r\n";
				if (sendMessage(client.getSocketFd(), response) == -1)
					return (EXIT_FAILURE);
				return (EXIT_SUCCESS);
			}
		}
		else {
			response = ":" + this->getHostname() + " " + ERR_NOSUCHCHANNEL + " " + client.getNickname() + " " + params[0] + " :No such channel\r\n";
			if (sendMessage(client.getSocketFd(), response) == -1)
				return (EXIT_FAILURE);
			return (EXIT_SUCCESS);
		}
	}
	
}

int Server::quit(std::vector<std::string> params, Client &client)
{
	std::string	response;

	if (params.size() == 0)
		response = ":" + client.getNickname() + "!" + client.getUsername() + "@" + client.getIpAddress() + " QUIT :Client Quit\r\n";
	else {
		for (size_t i = 0; i < params.size(); i++)
			response += params[i] + " ";
		if (response.empty())
			response.erase(response.size() - 1);
		response = ":" + client.getNickname() + "!" + client.getUsername() + "@" + client.getIpAddress() + " QUIT " + response + "\r\n";
	}
	for (std::vector<Channel>::iterator itChannel = _channels.begin(); itChannel != _channels.end(); ++itChannel) {
		if (itChannel->isClientInChannel(client) || itChannel->isOperatorInChannel(client)) {
			for (std::vector<Client>::iterator itClient = itChannel->_clients.begin(); itClient != itChannel->_clients.end(); ++itClient) {
				if (client.getNickname() != itClient->getNickname())
					if (sendMessage(itClient->getSocketFd(), response) == -1)
						return (EXIT_FAILURE);
			}
			for (std::vector<Client>::iterator itOperator = itChannel->_operators.begin(); itOperator != itChannel->_operators.end(); ++itOperator) {
				if (client.getNickname() != itOperator->getNickname())
					if (sendMessage(itOperator->getSocketFd(), response) == -1)
						return (EXIT_FAILURE);
			}
		}
	}
	cleanClientFromServer(client);
	return (EXIT_SUCCESS);
}

int	Server::kick(std::vector<std::string> params, Client &client)
{
	std::string	response;

	if (params.size() < 2) {
		response = ":" + this->getHostname() + " " + ERR_NEEDMOREPARAMS + " " + client.getNickname() + " KICK :Not enough parameters\r\n";
		if (sendMessage(client.getSocketFd(), response) == -1)
			return (EXIT_FAILURE);
		return (EXIT_SUCCESS);
	}
	std::vector<Channel>::iterator	itChannel = this->getChannelIterator(params[0]);
	if (itChannel != _channels.end())
	{
		if (itChannel->isOperatorInChannel(client.getNickname()))
		{
			std::vector<Client>::iterator	itClient = this->getClientIterator(params[1]);
			if (itClient != _clients.end())
			{
				if (itChannel->isClientInChannel(itClient->getNickname()))
				{
					if (params.size() == 2)
						response = ":" + client.getNickname() + "!" + client.getUsername() + "@" + client.getIpAddress() + " KICK " + params[0] + " " + params[1] + " :Kicked by " + client.getNickname() + "\r\n";
					else
					{
						for (size_t i = 2; i < params.size(); i++)
							response += params[i] + " ";
						if (response.empty())
							response.erase(response.size() - 1);
						response = ":" + client.getNickname() + "!" + client.getUsername() + "@" + client.getIpAddress() + " KICK " + params[0] + " " + params[1] + " " + response + "\r\n";
					}
					for (std::vector<Client>::iterator itClient = itChannel->_clients.begin(); itClient != itChannel->_clients.end(); ++itClient) {
						if (itChannel->isClientInChannel(itClient->getNickname()))
							if (sendMessage(itClient->getSocketFd(), response) == -1)
								return (EXIT_FAILURE);
					}
					for (std::vector<Client>::iterator itOperator = itChannel->_operators.begin(); itOperator != itChannel->_operators.end(); ++itOperator) {
						if (itChannel->isOperatorInChannel(itOperator->getNickname()))
							if (sendMessage(itOperator->getSocketFd(), response) == -1)
								return (EXIT_FAILURE);
					}
					itChannel->removeClient(*itClient);
					itChannel->removeOp(*itClient);
					return (EXIT_SUCCESS);
				}
				else
				{
					response = ":" + this->getHostname() + " " + ERR_USERNOTINCHANNEL + " " + client.getNickname() + " " + params[1] + " " + params[0] + " :They aren't on that channel\r\n";
					if (sendMessage(client.getSocketFd(), response) == -1 || sendMessage(itClient->getSocketFd(), response) == -1)
						return (EXIT_FAILURE);
					return (EXIT_SUCCESS);
				}
			}
			else
			{
				response = ":" + this->getHostname() + " " + ERR_NOSUCHNICK + " " + client.getNickname() + " " + params[1] + " :No such nickname\r\n";
				if (sendMessage(client.getSocketFd(), response) == -1)
					return (EXIT_FAILURE);
				return (EXIT_SUCCESS);
			}
		}
		else
		{
			response = ":" + this->getHostname() + " " + ERR_CHANOPRIVSNEEDED + " " + client.getNickname() + " " + params[0] + " :You're not channel operator\r\n";
			if (sendMessage(client.getSocketFd(), response) == -1)
				return (EXIT_FAILURE);
			return (EXIT_SUCCESS);
		}
	}
	else
	{
		response = ":" + this->getHostname() + " " + ERR_NOSUCHCHANNEL + " " + client.getNickname() + " " + params[0] + " :No such channel\r\n";
		if (sendMessage(client.getSocketFd(), response) == -1)
			return (EXIT_FAILURE);
		return (EXIT_SUCCESS);
	}

	//bool kick = false;
	/*kick = removeOp(victim);
	if (kick == false)
		kick = removeClient(victim);*/
}

int	Server::invalidCommand(std::string command, Client &client)
{
	std::string	response;
	std::transform(command.begin(), command.end(), command.begin(), ::toupper);

	if (client.isRegistered())
		response = ":" + this->getHostname() + " " + ERR_UNKNOWNCOMMAND + " " + client.getNickname() + " " + command + " :Unknown command\r\n";
	else
		response = ":" + this->getHostname() + " " + ERR_UNKNOWNCOMMAND + " * " + command + " :Unknown command\r\n";
	if (sendMessage(client.getSocketFd(), response) == -1)
			return (EXIT_FAILURE);
	return (-1);
}

bool	Server::checkChannelExists(std::string channelName)
{
	for (std::vector<Channel>::iterator it = _channels.begin(); it != _channels.end(); ++it)
		if (it->_name == channelName)
			return (true);
	return (false);
}

bool	Server::checkClientExists(std::string nickname)
{
	for (std::vector<Client>::iterator it = _clients.begin(); it != _clients.end(); ++it)
		if (it->getNickname() == nickname)
			return (true);
	return (false);
}

std::vector<Client>::iterator	Server::getClientIterator(std::string nickname)
{
	for (std::vector<Client>::iterator it = _clients.begin(); it != _clients.end(); ++it)
		if (it->getNickname() == nickname)
			return (it);
	return (_clients.end());
}

std::vector<Channel>::iterator	Server::getChannelIterator(std::string channelName)
{
	for (std::vector<Channel>::iterator it = _channels.begin(); it != _channels.end(); ++it)
		if (it->_name == channelName)
			return (it);
	return (_channels.end());
}

void Server::channelMode(std::string channelName, Client &client) {
	std::string response;

	for(std::vector<Channel>::iterator itChannel = _channels.begin(); itChannel != _channels.end(); ++itChannel) {
		if (itChannel->_name == channelName) {
			response = ":" + this->getHostname() + " " + RPL_CHANNELMODEIS + " " + client.getNickname() + " " + itChannel->_name + " " + itChannel->getMode() + "\r\n";
			if (sendMessage(client.getSocketFd(), response) == -1)
				return ;
		}
		else
			response = "Channel not found\n";
	}	

}

void Server::cleanClientFromServer(Client &client)
{
	std::vector<Client>::iterator itClient;

	//Close the socket and mark as 0 in list for reuse
	close(client.getSocketFd());
	FD_CLR(client.getSocketFd(), &_readFds);
	for (size_t j = 0; j < _channels.size(); j++)
	{
		_channels[j].removeClient(client);
		_channels[j].removeOp(client);
		itClient = this->getClientIterator(client.getNickname());
		if (itClient != _clients.end())
			_clients.erase(itClient);
	}
}

/* int Server::displayUserModes(std::string params, Client &client)
{
	std::string	response;
	std::string	mode;
	std::vector<std::string> modes;

	
	modes = client.getModes();

	for (size_t i = 0; i < modes.size(); i++)
		mode += modes[i];

	response = ":" + this->getHostname() + " " + RPL_UMODEIS + " " + client.getNickname() + " " + mode + "\r\n";
	if (sendMessage(client.getSocketFd(), response) == -1)
		return (EXIT_FAILURE);
	return (EXIT_SUCCESS);
} */

int Server::modeUser(std::vector<std::string> params, Client &client)
{
	std::string	response;
	std::vector<Channel>::iterator itChannel;
	std::vector<Client>::iterator itClient;

	itChannel = this->getChannelIterator(params[0]);
	if (itChannel != _channels.end())
	{
		if (itChannel->isOperatorInChannel(client))
		{
			if (params[1] == "+o" && params.size() == 3)
			{
				itClient = itChannel->findClient(params[2]);
				if (itClient != itChannel->_clients.end())
				{
					itChannel->addOperator(*itClient);
					itChannel->removeClient(*itClient);
					response = ":" + client.getNickname() + " MODE " + params[0] + " +o " + params[2] + "\r\n";
				}
				else if (!itChannel->isOperatorInChannel(params[2]))
					response = ":" + this->getHostname() + " " + ERR_USERNOTINCHANNEL + " " + client.getNickname() + " " + params[2] + " :They aren't on that channel\r\n";
				else
					return (EXIT_SUCCESS);
			}
			else if (params[1] == "-o" && params.size() == 3)
			{
				itClient = itChannel->findOperator(params[2]);
				if (itClient != itChannel->_clients.end())
				{
					itChannel->addClient(*itClient);
					itChannel->removeOp(*itClient);
					response = ":" + client.getNickname() + " MODE " + params[0] + " -o " + params[2] + "\r\n";
				}
				else
					response = ":" + this->getHostname() + " " + ERR_USERNOTINCHANNEL + " " + client.getNickname() + " " + params[2] + " :They aren't on that channel\r\n";
			}
			else if (params[1] == "+i" && params.size() == 2)
			{
				itChannel->addMode('i');
				response = ":" + client.getNickname() + " MODE " + params[0] + " +i\r\n";
			}
			else if (params[1] == "-i" && params.size() == 2)
			{
				itChannel->removeMode('i');
				response = ":" + client.getNickname() + " MODE " + params[0] + " -i\r\n";
			}
			else
			//TODO: check this reply
				return (this->invalidCommand("MODE " + params[1], client));
			for (std::vector<Client>::iterator itClient = itChannel->_clients.begin(); itClient != itChannel->_clients.end(); ++itClient)
			{
				if (sendMessage(itClient->getSocketFd(), response) == -1)
					return (EXIT_FAILURE);
			}
			for (std::vector<Client>::iterator itClient = itChannel->_operators.begin(); itClient != itChannel->_operators.end(); ++itClient)
			{
				if (sendMessage(itClient->getSocketFd(), response) == -1)
					return (EXIT_FAILURE);
			}
		}
		else {
			response = ":" + this->getHostname() + " " + ERR_CHANOPRIVSNEEDED + " " + client.getNickname() + " " + params[0] + " :You're not channel operator\r\n";
			if (sendMessage(client.getSocketFd(), response) == -1)
				return (EXIT_FAILURE);
		}
	}
	else
	{
		response = ":" + this->getHostname() + " " + ERR_NOSUCHCHANNEL + " " + client.getNickname() + " " + params[0] + " :No such channel\r\n";
		if (sendMessage(client.getSocketFd(), response) == -1)
			return (EXIT_FAILURE);
	}
	return (EXIT_SUCCESS);
}

int Server::handleNotRegistered(Client &client)
{
	std::string	response;

	response = ":" + this->getHostname() + " " + ERR_NOTREGISTERED + " " + client.getNickname() + " :You have not registered\r\n";
	if (sendMessage(client.getSocketFd(), response) == -1)
		return (EXIT_FAILURE);
	return (EXIT_SUCCESS);
}

int	Server::rpl_Welcome(const Client &client)
{
	std::string	response;

	response = ":" + this->getHostname() + " " + RPL_WELCOME + " " + client.getNickname() + " :" + WELCOME + " " + client.getNickname() + "!" + client.getUsername() + "@" + client.getIpAddress() + "\r\n";
	response += ":" + this->getHostname() + " " + RPL_YOURHOST + " " + client.getNickname() + " :Your host is " + this->getHostname() + ", running version 0.6\r\n";
	response += ":" + this->getHostname() + " " + RPL_CREATED + " " + client.getNickname() + " :This server was created sometime in the near future\r\n";
	// response += ":" + this->getHostname() + " 004 " + client.getNickname() + " " + this->getHostname() + " 0.6 insert channel modes\r\n";
	if (sendMessage(client.getSocketFd(), response) == -1)
		return (EXIT_FAILURE);
	return (EXIT_SUCCESS);
}

// Create a new channel
int	Server::createNewChannel(std::string channelName, Client &client) {
	std::string	response;

	if (sendMessage(client.getSocketFd(), response) == -1)
		return (EXIT_FAILURE);
	Channel newChannel(channelName, client);
	Server::_channels.push_back(newChannel);
	response = ":" + client.getNickname() + "!" + client.getUsername() + "@" + client.getIpAddress() + " MODE :" + channelName + " +o " + client.getNickname() + "\r\n";
	if (sendMessage(client.getSocketFd(), response) == -1)
		return (EXIT_FAILURE);
	return (EXIT_SUCCESS);
}

// Check if channel already exists
bool	Server::checkChannel(std::string channelName, Client &client){
	std::vector<Channel>::iterator it;
	for(it = Server::_channels.begin(); it != Server::_channels.end(); ++it) {
		if (it->_name == channelName)	{
			if (!it->isModeSet('i'))
				it->addClient(client);
			return (true);
		}
	}
	return (false);
}

int Server::joinChannel( std::vector<std::string> params, Client &client) {
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
	std::string	response;

	if (params.size() != 1)
	{
		response = ":" + this->getHostname() + " " + ERR_NEEDMOREPARAMS + " " + client.getNickname() + " JOIN :Incorrect number of arguments\r\n";
		if (sendMessage(client.getSocketFd(), response) == -1)
			return (EXIT_FAILURE);
		return (EXIT_SUCCESS);
	}

	// check if the client is registered
	if (!client.isRegistered())
	{
		if (client.getNickname().empty())
			response = ":" + this->getHostname() + " " + ERR_NOTREGISTERED + " * :You have not registered\r\n";
		else
			response = ":" + this->getHostname() + " " + ERR_NOTREGISTERED + " " + client.getNickname() + " :You have not registered\r\n";
		if (sendMessage(client.getSocketFd(), response) == -1)
			return (EXIT_FAILURE);
		return (EXIT_SUCCESS);
	}

	if (params[0].find(',') != std::string::npos)
	{
		response = ":" + this->getHostname() + " " + ERR_NEEDMOREPARAMS + " " + client.getNickname() + " JOIN :Invalid channel name (comma not allowed)\r\n";
		if (sendMessage(client.getSocketFd(), response) == -1)
			return (EXIT_FAILURE);
		return (EXIT_SUCCESS);
	}

	if (params[0].find('\a') != std::string::npos)
	{
		response = ":" + this->getHostname() + " " + ERR_NEEDMOREPARAMS + " " + client.getNickname() + " JOIN :Channel name cannot contain control G/BEL\r\n";
		if (sendMessage(client.getSocketFd(), response) == -1)
			return (EXIT_FAILURE);
		return (EXIT_SUCCESS);
	}

	// create a channel object and add it to the list of channels
	std::string channelName = params[0];

/** 
*	if channel name is valid check if the channel already exists
*	if not, create a new channel and send a reply to the JOIN command
*/
	if (params[0][0] == '#') {
		if (!checkChannel(channelName, client))
			createNewChannel(channelName, client);
		std::vector<Channel>::iterator itChannel = getChannel(channelName);
		if (!itChannel->isModeSet('i'))
			rpl_Join(client, *itChannel);
		else {
			if (itChannel->isInvited(client.getNickname())) {
				itChannel->addClient(client);
				itChannel->removeInvited(client.getNickname());
				rpl_Join(client, *itChannel);
			}
			else {
				response = ":" + this->getHostname() + " " + ERR_INVITEONLYCHAN + " " + client.getNickname() + " " + channelName + " :Cannot join channel (+i)\r\n";
				if (sendMessage(client.getSocketFd(), response) == -1)
					return (EXIT_FAILURE);
				return (EXIT_SUCCESS);
			}
		}
	}
	else  {
		response = channelName + ":No such channel\r\n";
		if (sendMessage(client.getSocketFd(), response) == -1)
			return (EXIT_FAILURE);
		return (EXIT_SUCCESS);
	}
	return (EXIT_SUCCESS);
}