#include "../includes/irc.hpp"

int main (int argc, char* argv[]) {
	
	if (argc != 3 ) // ./ircserv <port> <password>
	{
		std::cout << "Invalid command. Usage is: ./ircserv <port> <password>" << std::endl;
		return (-1);
	}

	bool	running = true;
	int		portNumber = std::atoi(argv[1]);
	Server	server(argv[2]);

	server.setHostname(SERVERNAME);
	try {
		server.setup_server(portNumber);
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		return (-1);
	}

	signal(SIGINT, handleSignal);

	// Create an array of client objects to store information about each connected client
	std::vector<Client> clients(MAX_CLIENTS, Client(0)); // initialize all elements with an invalid socket fd
	server.setClients(clients);

	while (running)
		running = server.run();
	server.clear_fd_set();
	clients.clear();

	return (0);
}
