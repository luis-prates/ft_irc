#include "../includes/irc.hpp"

/* std::vector<std::string>	split(std::string message, char del)
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
} */

void	handleSignal(int signal)
{
	if (signal == SIGINT)
	{
		std::cout << "\nSIGINT received. Exiting...\n";
		return;
	}
	return ;
}

int	sendMessage(const int socket, const std::string &message)
{
	if (send(socket, message.c_str(), message.size(), 0) == -1)
	{
		std::cout << "Error sending message" << std::endl;
		return (EXIT_FAILURE);
	}
	return (EXIT_SUCCESS);
}