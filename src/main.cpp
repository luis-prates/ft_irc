#include "../includes/irc.hpp"

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

		if (select(maxFd + 1, &readFds, NULL, NULL, NULL) < 0 && errno != EINTR)
		{
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
			printf("New connection , socket fd is %d , ip is : %s , port : %d \n",\
					newSocket , inet_ntoa(serverAddress.sin_addr), \
					ntohs(serverAddress.sin_port));
			
			//send new connection greeting message
			if( send(newSocket, message.c_str(), message.size(), 0) != message.size() )
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
					printf("Adding to list of sockets as %d\n" , i);

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
					//Somebody disconnected , get his details and print
					getpeername(sd , (struct sockaddr*)&serverAddress , \
						(socklen_t*)&addrlen);
					int rc = getsockname(sd , (struct sockaddr*)&serverAddress , \
						(socklen_t*)&addrlen);
					if (rc < 0)
					{
						std::cerr << "Error: Failed to get socket name\n";
						perror("getsockname");
					}
					printf("Host disconnected , ip %s , port %d \n" ,
							clients[i].getIpAddress() , clients[i].getPort());
							
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
					if (incomingMessage.find('\012') == std::string::npos)
					{
						std::cout << "\\r\\n was not found\n";
						std::cout << "Received message in string: " << incomingMessage << std::endl;
						std::cout << "Received message: " << buffer << std::endl;
					}
					else {
						std::cout << "Received message in string: " << incomingMessage;
						std::cout << "Received message: " << buffer;
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

	return (0);



	// reference chat client (irssi)
}
