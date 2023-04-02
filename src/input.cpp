#include "../includes/irc.hpp"

// needs refactoring and proper handling of commands
// used for testing with HexChat client
int	handle_user_input(std::string message, Client *client)
{
	std::string command;
	//TODO: is a vector the best way to store the parameters?
	std::vector<std::string> params;
	std::string response;
	// for debugging
	int dataSent = 0;
	int k = 0;

	// check if the message is a command
	// get the command
	command = message.substr(0, message.find(' '));
	std::transform(command.begin(), command.end(), command.begin(), ::tolower);
	// get the parameters
	std::cout << "command: " << command << std::endl;
	params = split(message.substr(message.find(' ') + 1), ' ');
	for (size_t i = 0; i < params.size(); i++)
		std::cout << "param: " << params[i] << std::endl;
	// check if the command is valid
	if (command == "nick")
	{
		// check if the nickname is valid
		// The syntax for this command is "NICK <nickname>". For example, "NICK John"
		if (params.size() != 1)
		{
			response = "Error: nickname cannot contain spaces\r\n";
			send(client->getSocketFd(), response.c_str(), response.size(), 0);
			return 0;
		}
		client->setNickname(params[0]);
		client->setRegistered(true);
		response = "Nickname set to " + params[0] + "\r\n";
		while (dataSent < response.size() && dataSent != -1)
		{
			std::cout << "atempt to send number: " << k++ << " to client " << client->getSocketFd() << "\n";
			dataSent = send(client->getSocketFd(), response.c_str(), response.size(), 0);
			if (dataSent == -1)
				std::cout << "error sending response\n";
		}
		return 0;
	}
	// The syntax for this command is "USER <username> <hostname> <servername> <realname>
	// For example, "USER John localhost irc.example.com John Doe".
	else if (command == "user")
	{
		if (params.size() < 4)
		{
			std::cout << "user commands params are invalid\n";
			response = "Error: invalid parameters\r\n";
			if (send(client->getSocketFd(), response.c_str(), response.size(), 0) == -1)
				std::cout << "error sending response\n";
			return 0;
		}
		// check if the username is valid
		response = "User set to " + params[0] + "\r\n";
		while (dataSent < response.size() && dataSent != -1)
		{
			dataSent = send(client->getSocketFd(), response.c_str(), response.size(), 0);
			if (dataSent == -1)
				std::cout << strerror(errno) << "\n";
		}

	}
	// The syntax for this command is "JOIN <channel>". For example, "JOIN #general"
	else if (command == "join")
	{
		// check if the channel name is valid
		if (params.size() != 1)
		{
			response = "Error: channel name cannot contain spaces\r\n";
			send(client->getSocketFd(), response.c_str(), response.size(), 0);
			return 0;
		}
		// check if the client is registered
		if (!client->isRegistered())
		{
			response = "Error: this is mine\r\n";
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
		//TODO check!
		client->setChannel(params[0]);
		//TODO check!
		response = "Joined channel " + params[0] + "\r\n";
		std::cout << response;
		if (send(client->getSocketFd(), response.c_str(), response.size(), 0) == -1)
				std::cout << "error sending response\n";
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
		if (params.size() == 0)
		{
			response = "Error: message cannot be empty\r\n";
			send(client->getSocketFd(), response.c_str(), response.size(), 0);
			return 0;
		}
		//TODO check message format
		response = client->getNickname() + ": " + params[0] + "\r\n";
		dataSent = send(client->getSocketFd(), response.c_str(), response.size(), 0);
		
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
		while (dataSent < response.size() && dataSent != -1)
		{
			dataSent = send(client->getSocketFd(), response.c_str(), response.size(), 0);
			if (dataSent == -1)
				std::cout << "error sending response\n";
		}
		return 0;
	}


	return 0;
}