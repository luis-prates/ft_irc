#include "../includes/irc.hpp"

int main (int argc, char* argv[]) {
	if (argc != 3 ) // ./ircserv <port> <password>
	{
		std::cout << "Invalid command. Usage is: ./ircserv <port> <password>" << std::endl;
		return -1;
	}

	int portNumber = std::atoi(argv[1]);
	int serverPassword = std::atoi(argv[2]);
	Server server;
	try {
		server.setup_server(portNumber);
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		return -1;
	}
	fd_set	readFds;
	int	sd;
	int	maxFd;
	int valread;
	char buffer[BUFFER_SIZE];

	signal(SIGINT, handleSignal);

	// Create an array of client objects to store information about each connected client
	std::vector<Client> clients(MAX_CLIENTS, Client(0)); // initialize all elements with an invalid socket fd

	while (true)
	{
		try
		{
			server.run(clients);
		}
		catch(const std::exception& e)
		{
			std::cerr << e.what() << '\n';
			server.clear_fd_set();
			clients.clear();
			break;
		}
	}

	return (0);

	// reference chat client (irssi)
}
