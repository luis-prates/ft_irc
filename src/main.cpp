#include "../includes/irc.hpp"

int main (int argc, char* argv[]) {
	if (argc != 3 ) // ./ircserv <port> <password>
	{
		std::cout << "Invalid command. Usage is: ./ircserv <port> <password>" << std::endl;
		return -1;
	}

	int portNumber = std::atoi(argv[1]);
	int serverPassword = std::atoi(argv[2]);
	std::cout << "port is: " << portNumber << std::endl;

	struct sockaddr_in serverAddress;
	serverAddress.sin_family = AF_INET;

	// INADDR_ANY is a special IP address that tells the socket to listen on all available network interfaces.
	serverAddress.sin_addr.s_addr = INADDR_ANY;
	serverAddress.sin_port = htons(portNumber);

	// create the socket
	int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	//TODO: check if more error handling is needed
	if (serverSocket < 0)
	{
		std::cerr << "Error: Failed to create the server socket\n";
		return (1);
	}

	// get the socket flags
	int flags = fcntl(serverSocket, F_GETFL, 0);
	if (flags < 0) {
		std::cerr << "Error: Failed to get the socket flags\n";
		return (1);
	}

	// set the socket to non-blocking mode
	if (fcntl(serverSocket, F_SETFL, flags | O_NONBLOCK) < 0)
	{
		std::cerr << "Error: Failed to set the socket to non-blocking mode\n";
		return (1);
	}

	if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)))
	{
		std::cerr << "Error: Failed to bind the server socket to a port\n";
		return (1);
	}

	if (listen(serverSocket, MAX_CONNECTIONS))
	{
		std::cerr << "Error: Failed to start listening for incoming connections\n";
		return 1;
	}


	// Create an array of client objects to store information about each connected client
	std::vector<Client> clients(MAX_CLIENTS, Client(-1)); // initialize all elements with an invalid socket fd

	fd_set	readFds;
	FD_ZERO(&readFds);
	FD_SET(serverSocket, &readFds);
	int	maxFd = serverSocket;

	std::cout << "Server started and listening for incoming connections on port " << portNumber << std::endl;

	while (true)
	{
		// Wait for new client connections
		fd_set	tempFds = readFds;
		if (select(maxFd + 1, &tempFds, NULL, NULL, NULL) == -1)
		{
			if (errno == EINTR)
			{
				continue; // select() interrupted by a signal, try again
			}
			std::cerr << "Error: Failed to select file descriptors\n";
			break;
		}
		for (int fd = 0; fd <= maxFd; fd++)
		{
			if (!FD_ISSET(fd, &tempFds))
			{
				continue;
			}
			if (fd == serverSocket)
			{
				// Handle incoming connection
				sockaddr_in	clientAddress;
				socklen_t	clientAddressSize = sizeof(clientAddress);
				int	clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddress, &clientAddressSize);
				if (clientSocket == -1)
				{
					//TODO: Error handling
					std::cerr << "Error: Failed to accept incoming connection\n";
					continue;
				}
				std::cout << "Client connected from " << inet_ntoa(clientAddress.sin_addr) << ":" << ntohs(clientAddress.sin_port) << std::endl;
				
				// get the client socket flags
				flags = fcntl(clientSocket, F_GETFL, 0);
				if (flags < 0) {
					std::cerr << "Error: Failed to get the socket flags\n";
					close(clientSocket);
					continue;
				}

				// set the client socket to non-blocking mode
				if (fcntl(clientSocket, F_SETFL, flags | O_NONBLOCK) < 0)
				{
					std::cerr << "Error: Failed to set the socket to non-blocking mode\n";
					close(clientSocket);
					continue;
				}

				// Add the client socket to the set of file descriptors to monitor
				bool addedClient = false;
				for (int i = 0; i < MAX_CLIENTS; i++)
				{
					if (clients[i].getSocketFd() == -1)
					{
						clients[i] = Client(clientSocket);
						FD_SET(clientSocket, &readFds);
						maxFd = std::max(maxFd, clientSocket);
						addedClient = true;
						break;
					}
				}
				if (!addedClient)
				{
					std::cerr << "Error: Too many clients connected\n";
					close(clientSocket);
				}
			}
			else
			{
				// Handles incoming data for an existing client
				Client& client = clients[fd];
				char buffer[BUFFER_SIZE];

				// Read from and write to the client socket:
				int	bytesReceived = recv(client.getSocketFd(), buffer, BUFFER_SIZE, 0);
				if (bytesReceived <= 0)
				{
					//TODO: Error handling
					// The client has disconnected or there was an error receiving data
					std::cout << "Client disconnected\n";
					close(client.getSocketFd());
					FD_CLR(client.getSocketFd(), &readFds);
					client = Client(-1); // reset the client object
				}
				else
				{
					// Append the received data to the client object
					client.getInputBuffer().append(buffer, bytesReceived);

					// Process the client's input buffer
					std::size_t newlinePos = client.getInputBuffer().find('\n');
					while (newlinePos != std::string::npos)
					{
						std::string message = client.getInputBuffer().substr(0, newlinePos + 1);
						client.getInputBuffer().erase(0, newlinePos + 1);
						std::cout << "Received message from client " << client.getNickname() << ": " << message;
						// Process the message and send a response back to the client
						client.getOutputBuffer() += "You said: " + message;
						newlinePos = client.getInputBuffer().find('\n');
					}
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