#include "../includes/irc.hpp"

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

void	handle_signal(int signal)
{
	if (signal == SIGINT)
	{
		std::cout << "SIGINT received. Exiting..." << std::endl;
		return;
	}
	return ;
	//stop = true;
}
