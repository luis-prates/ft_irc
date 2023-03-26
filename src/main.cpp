#include "../includes/irc.hpp"

void	handle_signal(int signal)
{
	if (signal == SIGINT)
	{
		std::cout << "SIGINT received. Exiting..." << std::endl;
		return;
	}
	return ;
}

std::vector<std::string>	split(std::string message, char del)
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
    while (getline(ss, s, del)) {
 
        // store token string in the vector
        v.push_back(s);
    }
	return (v);
}

// needs refactoring and proper handling of commands
// used for testing with HexChat client
int	handle_user_input(std::string message, Client *client)
{
	std::string command;
	std::string params;
	std::string response;

	// check if the message is a command
	// get the command
	command = message.substr(0, message.find(' '));
	std::transform(command.begin(), command.end(), command.begin(), ::tolower);
	// get the parameters
	std::cout << "command: " << command << std::endl;
	params = message.substr(message.find(' ') + 1);
	std::cout << "params: " << params << std::endl;
	// check if the command is valid
	if (command == "nick")
	{
		// check if the nickname is valid
		if (params.find(' ') != std::string::npos)
		{
			response = "Error: nickname cannot contain spaces\r\n";
			send(client->getSocketFd(), response.c_str(), response.size(), 0);
			return 0;
		}
		client->setNickname(params);
		client->setRegistered(true);
		response = "Nickname set to " + params + "\r\n";
		send(client->getSocketFd(), response.c_str(), response.size(), 0);
		return 0;
	}
	else if (command == "join")
	{
		// check if the channel name is valid
		if (params.find(' ') != std::string::npos)
		{
			response = "Error: channel name cannot contain spaces\r\n";
			send(client->getSocketFd(), response.c_str(), response.size(), 0);
			return 0;
		}
		// check if the client is registered
		if (!client->isRegistered())
		{
			response = "Error: you must set a nickname before joining a channel\r\n";
			send(client->getSocketFd(), response.c_str(), response.size(), 0);
			return 0;
		}
		// check if the client is already in a channel
		if (client->getChannel() != "")
		{
			response = "Error: you are already in a channel\r\n";
			send(client->getSocketFd(), response.c_str(), response.size(), 0);
			return 0;
		}
		client->setChannel(params);
		response = "Joined channel " + params + "\r\n";
		send(client->getSocketFd(), response.c_str(), response.size(), 0);
		return 0;
	}
	else if (command == "part")
	{
		// check if the client is registered
		if (!client->isRegistered())
		{
			response = "Error: you must set a nickname before leaving a channel\r\n";
			send(client->getSocketFd(), response.c_str(), response.size(), 0);
			return 0;
		}
		// check if the client is in a channel
		if (client->getChannel() == "")
		{
			response = "Error: you are not in a channel\r\n";
			send(client->getSocketFd(), response.c_str(), response.size(), 0);
			return 0;
		}
		response = "Left channel " + client->getChannel() + "\r\n";
		send(client->getSocketFd(), response.c_str(), response.size(), 0);
		client->setChannel("");
		return 0;
	}
	else if (command == "msg")
	{
		// check if the client is registered
		if (!client->isRegistered())
		{
			response = "Error: you must set a nickname before sending a message\r\n";
			send(client->getSocketFd(), response.c_str(), response.size(), 0);
			return 0;
		}
		// check if the client is in a channel
		if (client->getChannel() == "")
		{
			response = "Error: you are not in a channel\r\n";
			send(client->getSocketFd(), response.c_str(), response.size(), 0);
			return 0;
		}
		// check if the message is empty
		if (params == "")
		{
			response = "Error: message cannot be empty\r\n";
			send(client->getSocketFd(), response.c_str(), response.size(), 0);
			return 0;
		}
		response = client->getNickname() + ": " + params + "\r\n";
		send(client->getSocketFd(), response.c_str(), response.size(), 0);
		return 0;
	}
	else if (command == "list")
	{
		// check if the client is registered
		if (!client->isRegistered())
		{
			response = "Error: you must set a nickname before listing channels\r\n";
			send(client->getSocketFd(), response.c_str(), response.size(), 0);
			return 0;
		}
		// check if the client is in a channel
		if (client->getChannel() == "")
		{
			response = "Error: you are not in a channel\r\n";
			send(client->getSocketFd(), response.c_str(), response.size(), 0);
			return 0;
		}
		response = "List of channels:\r\n";
		send(client->getSocketFd(), response.c_str(), response.size(), 0);
		return 0;
	}
	else if (command == "names")
	{
		// check if the client is registered
		if (!client->isRegistered())
		{
			response = "Error: you must set a nickname before listing users\r\n";
			send(client->getSocketFd(), response.c_str(), response.size(), 0);
			return 0;
		}
		// check if the client is in a channel
		if (client->getChannel() == "")
		{
			response = "Error: you are not in a channel\r\n";
			send(client->getSocketFd(), response.c_str(), response.size(), 0);
			return 0;
		}
		response = "List of users:\r\n";
		send(client->getSocketFd(), response.c_str(), response.size(), 0);
		return 0;
	}
	else if (command == "quit")
	{
		// check if the client is registered
		if (!client->isRegistered())
		{
			response = "Error: you must set a nickname before quitting\r\n";
			send(client->getSocketFd(), response.c_str(), response.size(), 0);
			return 0;
		}
		response = "Goodbye!\r\n";
		send(client->getSocketFd(), response.c_str(), response.size(), 0);
		return 1;
	}
	else
	{
		response = "Error: invalid command\r\n";
		send(client->getSocketFd(), response.c_str(), response.size(), 0);
		return 0;
	}


	return 0;
}

int main (int argc, char* argv[]) {
	if (argc != 3 ) // ./ircserv <port> <password>
	{
		std::cout << "Invalid command. Usage is: ./ircserv <port> <password>" << std::endl;
		return -1;
	}

	int portNumber = std::atoi(argv[1]);
	int serverPassword = std::atoi(argv[2]);
	int	opt = TRUE;
	struct sockaddr_in serverAddress;
	int serverSocket;
	int	newSocket;
	int addrlen = sizeof(serverAddress);
	fd_set	readFds;
	int	sd;
	int	maxFd;
	int valread;
	char buffer[BUFFER_SIZE];
	std::string message("Welcome to my IRC server!\r\n");
	std::cout << "port is: " << portNumber << std::endl;

	//signal(SIGINT, handle_signal);

	// create the socket
	//TODO: check if more error handling is needed
	if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == 0)
	{
		std::cerr << "Error: Failed to create the server socket\n";
		perror("socket failed");
		exit (EXIT_FAILURE);
	}

	//set master socket to allow multiple connections , 
	//this is just a good habit, it will work without this 
	if( setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, 
			sizeof(opt)) < 0 )  
	{  
		perror("setsockopt");  
		exit(EXIT_FAILURE);  
	}  

	serverAddress.sin_family = AF_INET;
	// INADDR_ANY is a special IP address that tells the socket to listen on all available network interfaces.
	serverAddress.sin_addr.s_addr = INADDR_ANY;
	serverAddress.sin_port = htons(portNumber);

	if (bind(serverSocket, (struct sockaddr *)&serverAddress, addrlen) < 0)
	{
		std::cerr << "Error: Failed to bind the server socket to a port\n";
		perror("bind failed");
		exit(EXIT_FAILURE);
	}

	if (listen(serverSocket, MAX_CONNECTIONS) < 0)
	{
		std::cerr << "Error: Failed to start listening for incoming connections\n";
		perror("listen");
		exit(EXIT_FAILURE);
	}

	addrlen = sizeof(serverAddress);

	// Create an array of client objects to store information about each connected client
	std::vector<Client> clients(MAX_CLIENTS, Client(0)); // initialize all elements with an invalid socket fd

	

	std::cout << "Server started and listening for incoming connections on port " << portNumber << std::endl;

	while (true)
	{
		// clear the socket set
		FD_ZERO(&readFds);
		// add server socket to set
		FD_SET(serverSocket, &readFds);
		maxFd = serverSocket;

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
		if (select(maxFd + 1, &readFds, NULL, NULL, NULL) < 0 && errno != EINTR)
		{
			std::cout << "Error: Failed to select file descriptors\n";
			std::cerr << "Error: Failed to select file descriptors\n";
		}
		//If something happened on the master socket,
		//then its an incoming connection
		if (FD_ISSET(serverSocket, &readFds))
		{
			if ((newSocket = accept(serverSocket,
					(struct sockaddr *)&serverAddress, (socklen_t*)&addrlen)) < 0)
			{
				perror("accept");
				exit(EXIT_FAILURE);
			}
				
			//inform user of socket number - used in send and receive commands
			std::cout << "New connection, socket fd is " << newSocket << \
					", ip is: " << inet_ntoa(serverAddress.sin_addr) << \
					", port: " << ntohs(serverAddress.sin_port) << std::endl;
			
			//send new connection greeting message
			if( send(newSocket, "Welcome to my IRC server!\r\n", 28, 0) != 28 )
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
					clients[i].setSocketFd(newSocket);
					clients[i].setIpAddress(inet_ntoa(serverAddress.sin_addr));
					clients[i].setPort(ntohs(serverAddress.sin_port));
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
				if ((valread = read( sd , buffer, 1024)) == 0)
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
						valread = read( sd , buffer, 1024);
						incomingMessage.append(buffer, valread);
					}
					std::cout << "Message complete. Message is: " << incomingMessage;
					std::vector<std::string> commands = split(incomingMessage, '\012');
					for (size_t i = 0; i < commands.size(); i++)
					{
						std::cout << "Command: " << commands[i] << std::endl;
						handle_user_input(commands[i], &clients[i]);
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
	close(serverSocket);
	clients.clear();

	return (0);



	// reference chat client (irssi)
}
