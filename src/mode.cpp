#include "../includes/Server.hpp"
#include "../includes/Channel.hpp"
#include "../headers/Macros.hpp"

std::string Channel::getMode() {
	std::string response = "+";
	for (std::vector<char>::iterator it = this->_mode.begin(); it != this->_mode.end(); ++it) {
		if (*it == ':') break;
		response += *it;
	}
	return response;
}

void Server::mode(std::string channel_name, Client &client) {
	std::string response;
	for(std::vector<Channel>::iterator it = _channels.begin(); it != _channels.end(); ++it) {
		if (it->_name == channel_name) {
			response = ":" + this->getHostname() + " 324 " + client.getNickname() + " " + it->_name + " " + it->getMode() + "\r\n";
			if (sendMessage(client.getSocketFd(), response) == -1)
				return ;
		}
		else
			response = "Channel not found\n";
	}	

}