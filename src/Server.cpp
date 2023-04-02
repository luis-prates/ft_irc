#include "../includes/Server.hpp"

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
	fd_reset_n_set(clients);

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
		handle_new_connection(clients);

	//else it's some IO operation on some other socket
	for (int i = 0; i < MAX_CLIENTS; i++)
	{
		if (FD_ISSET(clients[i].getSocketFd(), &_readFds))
		{
			//Check if it was for closing, and also read the
			//incoming message
			if (handle_client_input(clients[i]) == 0)
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


int Server::fd_reset_n_set(std::vector<Client> &clients)
{
	int	clientFd;

	// clear the socket set
	FD_ZERO(&_readFds);
	// add server socket to set
	FD_SET(this->getSocket(), &_readFds);
	_maxFd = this->getSocket();

	//add child sockets to set 
	for ( int i = 0 ; i < MAX_CLIENTS ; i++)
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

int Server::handle_new_connection(std::vector<Client> &clients)
{
	t_socket tmpSocket;
	std::string message("Welcome to my IRC server!\r\n");

	if ((tmpSocket.socket = accept(this->getSocket(),
			(struct sockaddr *)&tmpSocket.address, (socklen_t*)&tmpSocket.addrlen)) < 0 && errno == EINTR)
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
			
	puts("Welcome message sent successfully");
			
	//add new socket to array of sockets
	for (int i = 0; i < MAX_CLIENTS; i++)
	{
		//if position is empty 
		if( clients[i].getSocketFd() == 0 )
		{
			clients[i].setSocketFd(tmpSocket.socket);
			clients[i].setIpAddress(inet_ntoa(tmpSocket.address.sin_addr));
			clients[i].setPort(ntohs(tmpSocket.address.sin_port));
			std::cout << "Adding to list of sockets as " << i << std::endl;
			break;
		}
	}
	return (0);
}

int Server::handle_client_input(Client &client)
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
		if (client.getOutputBuffer().find('\012') == std::string::npos)
		{
			std::cout << "\\r\\n was not found\n";
			std::cout << "Received message: " << buffer << std::endl;
			std::cout << "Output buffer: " << client.getOutputBuffer() << std::endl;
		}
		else
		{
			std::cout << "Message complete. Message is: " << client.getOutputBuffer() << std::endl;
			std::vector<std::string> commands = split(client.getOutputBuffer(), '\012');
			for (size_t k = 0; k < commands.size(); k++)
			{
				std::cout << "Command: " << commands[k] << std::endl;
				handle_user_input(commands[k], &client);
			}
			client.getOutputBuffer().clear();
		}
	}
	return (0);
}
