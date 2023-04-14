#include "../includes/Server.hpp"
#include "../includes/Channel.hpp"
#include "../headers/Macros.hpp"

Server::Server()
{
	
}


Server::Server(int port)
{
	
}

Server::~Server()
{
	
}

int Server::setup_server(int port)
{
	int sockOpt = 1;

	_addrlen = sizeof(_address);
	// create the socket
	//TODO: check if more error handling is needed
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

	return (0);
}

int Server::run(std::vector<Client> &clients)
{
	fdResetNSet(clients);

	std::cout << "Waiting for new connections...\n";
	if (select(_maxFd + 1, &_readFds, NULL, NULL, NULL) < 0 && errno == EINTR)
	{
		perror("select");
		std::cerr << "Error: Failed to select file descriptors\n";
		throw ServerException();
	}
	//If something happened on the master socket,
	//then its an incoming connection
	if (FD_ISSET(this->getSocket(), &_readFds))
		handleNewConnection(clients);

	//else it's some IO operation on some other socket
	for (int i = 0; i < MAX_CLIENTS; i++)
	{
		if (FD_ISSET(clients[i].getSocketFd(), &_readFds))
		{
			//Check if it was for closing, and also read the
			//incoming message
			if (handleClientInput(clients[i]) == 0)
			{
				//Echo back the message that came in
				std::cout << "Handled client input with successs\n";
				//send(clientFd, message.c_str(), message.size(), 0);
			}
		}
	}
	return (0);
}

int Server::clear_fd_set()
{
	// Clean up
	for (int fd = 0; fd <= _maxFd; fd++)
	{
		if (FD_ISSET(fd, &_readFds))
		{
			close(fd);
		}
	}
	close(this->getSocket());
	return (0);
}


int Server::fdResetNSet(std::vector<Client> &clients)
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
	return (0);
}

int Server::handleNewConnection(std::vector<Client> &clients)
{
	t_socket tmpSocket;
	tmpSocket.addrlen = sizeof(tmpSocket.address);
	std::string message(WELCOME);

	if ((tmpSocket.socket = accept(this->getSocket(),
			(struct sockaddr *)&tmpSocket.address, (socklen_t*)&tmpSocket.addrlen)) < 0 /*&& errno == EINTR*/)
	{
		perror("accept");
		throw ServerException();
	}
		
	//inform user of socket number - used in send and receive commands
	std::cout << "New connection, socket fd is " << tmpSocket.socket << \
			", ip is: " << inet_ntoa(tmpSocket.address.sin_addr) << \
			", port: " << ntohs(tmpSocket.address.sin_port) << std::endl;
	
	//send new connection greeting message
	if(send(tmpSocket.socket, message.c_str(), message.size(), 0) != message.size())
	{
		perror("send");
		throw ServerException();
	}

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
	return (0);
}

int Server::handleClientInput(Client &client)
{
	int valread;
	char buffer[BUFFER_SIZE];

	//Check if it was for closing , and also read the 
	//incoming message
	if ((valread = recv(client.getSocketFd(), buffer, 1024, 0)) == 0)
	{
		std::cout << "Host disconnected, ip: " << client.getIpAddress() << \
				" port:" << client.getPort() << std::endl;
				
		//Close the socket and mark as 0 in list for reuse
		close(client.getSocketFd());
		FD_CLR(client.getSocketFd(), &_readFds);
		client.setSocketFd(0);
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
			std::cout << "Message complete. Message is: " << client.getOutputBuffer() << std::endl;
			std::vector<std::string> commands = split(client.getOutputBuffer(), '\012');
			for (size_t k = 0; k < commands.size(); k++)
			{
				commands[k].erase(std::find(commands[k].begin(), commands[k].end(), '\r'), commands[k].end());
				std::cout << "Command: " << commands[k] << std::endl;
				handleCommands(commands[k], client);
			}
			client.getOutputBuffer().clear();
		}
		else
		{
			std::cout << "\\r\\n was not found\n";
			std::cout << "Received message: " << buffer << std::endl;
			std::cout << "Output buffer: " << client.getOutputBuffer() << std::endl;
		}
	}
	return (0);
}

// needs refactoring and proper handling of commands
// used for testing with HexChat client
int Server::handleCommands(std::string message, Client &client)
{
	std::string command;
	std::vector<std::string> params;
	std::string response;

	// check if the message is a command
	// get the command
	command = message.substr(0, message.find(' '));
	std::transform(command.begin(), command.end(), command.begin(), ::tolower);
	// get the parameters
	std::cout << "command: " << command << std::endl;
	params = split(message.substr(message.find(' ') + 1), ' ');
	for (size_t i = 0; i < params.size(); i++) {
		std::cout << "param: " << params[i] << std::endl;
	}
	// check if the command is valid
	if (command == "nick")
	{
		if (handleNick(params, client) == -1)
			return (-1);
	}
	// The syntax for this command is "USER <username> <hostname> <servername> <realname>
	// For example, "USER John localhost irc.example.com John Doe".
	else if (command == "user")
	{
		if (params.size() < 4)
		{
			std::cout << "user commands params are invalid\n";
			response = "Error: invalid parameters\r\n";
			if (send(client.getSocketFd(), response.c_str(), response.size(), 0) == -1)
				std::cout << "error sending response\n";
			return (0);
		}
		// check if the username is valid
		response = "User set to " + params[0] + "\r\n";
		//TODO: may need to protect this better
		if (send(client.getSocketFd(), response.c_str(), response.size(), 0) == -1)
				std::cout << "error sending response\n";
	}
	// The syntax for this command is "JOIN <channel>". For example, "JOIN #general"
	else if (command == "join")
		return (joinChannel(params, client, response));
	else if (command == "who")
		who(params, client);
	else if (command == "privmsg")
		privmsg(params, client);
	else if (command == "part")
	{
		// check if the client is registered
		if (!client.isRegistered())
		{
			response = "Error: you must set a nickname before leaving a channel\r\n";
			//TODO: may need to protect this better
			if (send(client.getSocketFd(), response.c_str(), response.size(), 0) == -1)
				std::cout << "error sending response\n";
			return (0);
		}
		// check if the client is in a channel
		if (client.getChannel() == "")
		{
			response = "Error: you are not in a channel\r\n";
			//TODO: may need to protect this better
			if (send(client.getSocketFd(), response.c_str(), response.size(), 0) == -1)
				std::cout << "error sending response\n";
			return (0);
		}
		response = ":" + client.getNickname() + "!" + client.getNickname() + "@" + client.getIpAddress() + " PART " + client.getChannel() + " :Leaving\r\n";
		//TODO: may need to protect this better
		if (send(client.getSocketFd(), response.c_str(), response.size(), 0) == -1)
				std::cout << "error sending response\n";
		client.addChannel("");
		return (0);
	}
	else if (command == "msg")
	{
		// check if the client is registered
		if (!client.isRegistered())
		{
			response = "Error: you must set a nickname before sending a message\r\n";
			//TODO: may need to protect this better
			if (send(client.getSocketFd(), response.c_str(), response.size(), 0) == -1)
				std::cout << "error sending response\n";
			return (0);
		}
		// check if the client is in a channel
		if (client.getChannel() == "")
		{
			response = "Error: you are not in a channel\r\n";
			//TODO: may need to protect this better
			if (send(client.getSocketFd(), response.c_str(), response.size(), 0) == -1)
				std::cout << "error sending response\n";
			return (0);
		}
		// check if the message is empty
		if (params.size() == 0)
		{
			response = "Error: message cannot be empty\r\n";
			//TODO: may need to protect this better
			if (send(client.getSocketFd(), response.c_str(), response.size(), 0) == -1)
				std::cout << "error sending response\n";
			return (0);
		}
		//TODO check message format
		response = client.getNickname() + ": " + params[0] + "\r\n";
		//TODO: may need to protect this better
		if (send(client.getSocketFd(), response.c_str(), response.size(), 0) == -1)
				std::cout << "error sending response\n";
		
		return (0);
	}
	else if (command == "quit")
	{
		// check if the client is registered
		if (!client.isRegistered())
		{
			response = "Error: you must set a nickname before quitting\r\n";
			//TODO: may need to protect this better
			if (send(client.getSocketFd(), response.c_str(), response.size(), 0) == -1)
				std::cout << "error sending response\n";
			return (0);
		}
		response = "Goodbye!\r\n";
		//TODO: may need to protect this better
		if (send(client.getSocketFd(), response.c_str(), response.size(), 0) == -1)
				std::cout << "error sending response\n";
		return 1;
	}
	else
	{
		response = "Error: invalid command\r\n";
		//TODO: may need to protect this better
		if (send(client.getSocketFd(), response.c_str(), response.size(), 0) == -1)
				std::cout << "error sending response\n";
		return (0);
	}


	return (0);
}

std::vector<std::string> Server::split(std::string message, char del)
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

int Server::handleNick(std::vector<std::string> params, Client &client)
{
	std::string response;

	// check if the nickname is valid
	// The syntax for this command is "NICK <nickname>". For example, "NICK John"
	if (params.size() != 1)
	{
		response = "Error: nickname cannot contain spaces\r\n";
		send(client.getSocketFd(), response.c_str(), response.size(), 0);
		return (0);
	}
	client.setNickname(params[0]);
	client.setRegistered(true);
	response = "Nickname set to " + params[0] + "\r\n";
	//TODO: may need to protect this better
	if (send(client.getSocketFd(), response.c_str(), response.size(), 0) == -1)
			std::cout << "error sending response\n";
	return (0);
}



// Reply to the client to confirm the join
void Server::rpl_Join(Client client, Channel newChannel, std::string response) {
	response = ":" + client.getNickname() + " JOIN " + newChannel.getName() + "\r\n";
	if (send(client.getSocketFd(), response.c_str(), response.size(), 0) == -1)
		std::cout << "error sending response\n";
	// RPL_TOPIC
	if (newChannel.getTopic() != "")
		response = newChannel.getName() + " :" + newChannel.getTopic() + "\r\n";
	else
		response = newChannel.getName() + " :No topic is set\r\n";
	if (send(client.getSocketFd(), response.c_str(), response.size(), 0) == -1)
		std::cout << "error sending response\n";
}

Channel* Server::getChannel(std::string channelName) {
	for(std::vector<Channel>::iterator it = _channels.begin(); it != _channels.end(); ++it) {
		if (it->_name == channelName)	{
			return &(*it);
		}
	}
	return 0;
}

// Not working as expected... thought that if I gave the correct reply the hexchat would list my user in the channel
//in the right side bar... but it doesn't
void Server::who(std::vector<std::string> params, Client &client) {
	std::string responseNames;
	std::string responseWho;
	for(std::vector<Channel>::iterator it = _channels.begin(); it != _channels.end(); ++it) {
		std::cout << "Channel: " << it->getName() << "\n";
		if (it->_name == params[0])	{
			for (std::vector<Client>::iterator it2 = it->_clients.begin(); it2 != it->_clients.end(); ++it2) {
				// :hostname 353 nickname = #channel :nickname nickname (can be more than one here or sent in multiple messages)
				responseNames += ":mine_test 353 " + client.getNickname() + " = " + it->getName() + " " + it2->getNickname() + "\r\n";
				// :hostname 354 nickname #channel nickname userIpAddress hostname nickname channelModes hopcount(0 for single server) :realname
				responseWho += ":mine_test 354 " + client.getNickname() + " " + it->getName() + " " + it2->getNickname() + " " + it2->getIpAddress() + " mine_test " + it2->getNickname() + " H 0 :luism\r\n";
			}
			// :hostname 366 nickname #channel :End of /NAMES list.
			responseNames += ":mine_test 366 " + client.getNickname() + " " + it->getName() + " :End of /NAMES list.\r\n";
			// :hostname 315 nickname #channel :End of /WHO list.
			responseWho += ":mine_test 315 " + client.getNickname() + " " + it->getName() + " :End of /WHO list.\r\n";
			// concatenate the two strings
			responseNames += responseWho;
			//response += ":" + it->getName() + " 352 " + client.getNickname() + " " + it->_name + " :End of WHO list.\r\n";
			if (send(client.getSocketFd(), responseNames.c_str(), responseNames.size(), 0) == -1)
				std::cout << "error sending response\n";
		}
	}
}


/** Still needs to be tested with more than one channel 
 * 	PRIVMSG messages to other users outside the channel is not implemented
*/
void Server::privmsg(std::vector<std::string> params, Client &client) {
	// check if the channel exists
	std::string response;
	std::string msg;
	for(std::vector<Channel>::iterator it = _channels.begin(); it != _channels.end(); ++it) {
		/** @bug the + '\r' is a patch that may not work in all ocasions
		 * the comparison is not working as expected */
		if (it->_name == params[0] + '\r')	{
			// message to channel
 			for (int i = 1; i < params.size(); i++)
 					msg += params[i] + " ";
			for (std::vector<Client>::iterator it2 = it->_clients.begin(); it2 != it->_clients.end(); ++it2) {

				/** @bug the second " :" is necessary to send messages to all clients
				 * but the two dots are being appended to the message also
				 * so the message is being sent as " :message"
				*/
				// Reply to the client to send message to channel
 				response = ":" + client.getNickname() + " PRIVMSG " + it->getName() + " :" + msg + "\r\n";

				// Don't send the response to the sender
				if (client.getNickname() != it2->getNickname())
					if (send(it2->getSocketFd(), response.c_str(), response.size(), 0) == -1)
						std::cout << "error sending response\n";
			}
		}
	}
}