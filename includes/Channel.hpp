#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <string>
# include <vector>
# include "Client.hpp"

class Channel
{
private:
	std::string	_name;
	std::vector<Client*> clients;
	std::vector<Client*> operators;
	
public:
	Channel(/* args */);
	~Channel();
};

Channel::Channel(/* args */)
{
}

Channel::~Channel()
{
}


#endif