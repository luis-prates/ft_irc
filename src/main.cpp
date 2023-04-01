#include "../includes/irc.hpp"

int main (int argc, char* argv[]) {
	if (argc != 3 ) // ./ircserv <port> <password>
	{
		std::cout << "Invalid command. Usage is: ./ircserv <port> <password>" << std::endl;
		return -1;
	}

	int portNumber = std::atoi(argv[1]);
	int serverPassword = std::atoi(argv[2]);
	t_server client;
	Server server;
	try {
		server.setup_server(portNumber);
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		return -1;
	}
	client.addrlen = sizeof(client.address);
	fd_set	readFds;
	int	sd;
	int	maxFd;
	int valread;
	char buffer[BUFFER_SIZE];
	std::string message("Welcome to my IRC server!\r\n");

	signal(SIGINT, handle_signal);

	// Create an array of client objects to store information about each connected client
	std::vector<Client> clients(MAX_CLIENTS, Client(0)); // initialize all elements with an invalid socket fd

	while (true)
	{
		// clear the socket set
		FD_ZERO(&readFds);
		// add server socket to set
		FD_SET(server.getSocket(), &readFds);
		maxFd = server.getSocket();

		// Wait for new client connections
		//fd_set	tempFds = readFds;

		//add child sockets to set 
		for ( int i = 0 ; i < MAX_CLIENTS ; i++)
		{
			//socket descriptor 
			sd = clients[i].getSocketFd();

			//if valid socket descriptor then add to read list
			if(sd > 0)
				FD_SET( sd , &readFds);
					
			//highest file descriptor number, need it for the select function
			if(sd > maxFd)
				maxFd = sd;
		}

		// this needs to be handled better, it causes a leak
		// because it doesn't free the memory allocated in the
		// beginning of the program, the string message and the clients vector
		// it exits abruptly after SIGINT is received
		std::cout << "Waiting for new connections...\n";
		if (select(maxFd + 1, &readFds, NULL, NULL, NULL) < 0 && errno == EINTR)
		{
			perror("select");
			std::cerr << "Error: Failed to select file descriptors\n";
			break;
		}
		std::cout << "select returned\n";
		//If something happened on the master socket,
		//then its an incoming connection
		if (FD_ISSET(server.getSocket(), &readFds))
		{
			std::cout << "new connection\n";
			if ((client.socket = accept(server.getSocket(),
					(struct sockaddr *)&client.address, (socklen_t*)&client.addrlen)) < 0 && errno == EINTR)
			{
				perror("accept");
				exit(EXIT_FAILURE);
			}
				
			//inform user of socket number - used in send and receive commands
			std::cout << "New connection, socket fd is " << client.socket << \
					", ip is: " << inet_ntoa(client.address.sin_addr) << \
					", port: " << ntohs(client.address.sin_port) << std::endl;
			
			//send new connection greeting message
			if( send(client.socket, message.c_str(), message.size(), 0) != message.size() )
			{
				perror("send");
			}
					
			puts("Welcome message sent successfully");
					
			//add new socket to array of sockets
			for (int i = 0; i < MAX_CLIENTS; i++)
			{
				//if position is empty 
				if( clients[i].getSocketFd() == 0 )
				{
					clients[i].setSocketFd(client.socket);
					clients[i].setIpAddress(inet_ntoa(client.address.sin_addr));
					clients[i].setPort(ntohs(client.address.sin_port));
					std::cout << "Adding to list of sockets as " << i << std::endl;

					break;
				}
			}
		}
		//else its some IO operation on some other socket
		for (int i = 0; i < MAX_CLIENTS; i++)
		{
			sd = clients[i].getSocketFd();
					
			if (FD_ISSET( sd , &readFds))
			{  
				//Check if it was for closing , and also read the 
				//incoming message
				if ((valread = recv( sd , buffer, 1024, 0)) == 0)
				{
					std::cout << "Host disconnected, ip: " << clients[i].getIpAddress() << \
							" port:" << clients[i].getPort() << std::endl;
							
					//Close the socket and mark as 0 in list for reuse
					close( sd );
					FD_CLR(clients[i].getSocketFd(), &readFds);
					clients[i].setSocketFd(0);
				}
				//Echo back the message that came in 
				else 
				{
					//set the string terminating NULL byte on the end 
					//of the data read
					buffer[valread] = '\0';
					// Handle the incoming message
					std::string incomingMessage(buffer, valread);
					// '\012' represents \r\n
					while (incomingMessage.find('\012') == std::string::npos)
					{
						std::cout << "\\r\\n was not found\n";
						std::cout << "Received message in string: " << incomingMessage << std::endl;
						std::cout << "Received message: " << buffer << std::endl;
						valread = recv( sd , buffer, 1024, 0);
						incomingMessage.append(buffer, valread);
					}
					std::cout << "Message complete. Message is: " << incomingMessage;
					std::vector<std::string> commands = split(incomingMessage, '\012');
					for (size_t k = 0; k < commands.size(); k++)
					{
						std::cout << "Command: " << commands[k] << std::endl;
						handle_user_input(commands[k], &clients[i]);
					}
					
					//send(sd , buffer , strlen(buffer) , 0 );
				}
			}
		}
	}

	// Clean up
	for (int fd = 0; fd <= maxFd; fd++)
	{
		if (FD_ISSET(fd, &readFds))
		{
			close(fd);
		}
	}
	close(server.getSocket());
	clients.clear();

	return (0);



	// reference chat client (irssi)
}
