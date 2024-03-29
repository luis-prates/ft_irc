#include "../includes/Channel.hpp"
#include "../includes/Server.hpp"

void Channel::addClient(Client client) {
	_clients.push_back(client);
}

void Channel::addOperator(Client op) {
	_operators.push_back(op);
}

bool	Channel::removeClient(Client client) {
	for (std::vector<Client>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
		if (it->getNickname() == client.getNickname()) {
			_clients.erase(it);
			return (true);
		}
	}
	return (false);
}

bool Channel::removeOp(Client op)	{
	for (std::vector<Client>::iterator it = _operators.begin(); it != _operators.end(); ++it) {
		if (it->getNickname() == op.getNickname()) {
			_operators.erase(it);
			return (true);
		}
	}
	return (false);
}

std::string Channel::getTopic() { return (_topic); }

void Channel::setTopic(std::string topic) { _topic = topic; }

std::string Channel::getName() { return (_name); }

std::string Channel::getMode() {
	std::string response = "+";

	for (std::vector<char>::iterator it = this->_mode.begin(); it != this->_mode.end(); ++it) {
		if (*it == ':') break;
		response += *it;
	}
	return response;
}

std::vector<char> Channel::getModeVector() { return (_mode); }

void	Channel::addMode(char mode) {
	std::vector<char>::iterator	itChannel;

	for (itChannel = this->_mode.begin(); itChannel != this->_mode.end(); ++itChannel) {
		if (*itChannel == mode)
			break;
	}
	if (itChannel == this->_mode.end())
		this->_mode.push_back(mode);
}

void	Channel::removeMode(char mode) {
	std::vector<char>::iterator	itChannel;

	for (itChannel = this->_mode.begin(); itChannel != this->_mode.end(); ++itChannel) {
		if (*itChannel == mode)
			break;
	}
	if (itChannel != this->_mode.end())
		this->_mode.erase(itChannel);
}

bool	Channel::isClientInChannel(Client client) {
	for (std::vector<Client>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
		if (it->getNickname() == client.getNickname())
			return (true);
	}
	return (false);
}

bool	Channel::isClientInChannel(std::string nickname) {
	for (std::vector<Client>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
		if (it->getNickname() == nickname)
			return (true);
	}
	return (false);
}

bool	Channel::isOperatorInChannel(Client client) {
	for (std::vector<Client>::iterator it = _operators.begin(); it != _operators.end(); ++it) {
		if (it->getNickname() == client.getNickname())
			return (true);
	}
	return (false);
}

bool	Channel::isOperatorInChannel(std::string nickname) {
	for (std::vector<Client>::iterator it = _operators.begin(); it != _operators.end(); ++it) {
		if (it->getNickname() == nickname)
			return (true);
	}
	return (false);
}

bool	Channel::isModeSet(char mode) {
	for (std::vector<char>::iterator it = this->_mode.begin(); it != this->_mode.end(); ++it) {
		if (*it == mode)
			return (true);
	}
	return (false);
}

bool	Channel::isInvited(Client client) {
	for (std::vector<std::string>::iterator it = _invited.begin(); it != _invited.end(); ++it) {
		if (*it == client.getNickname())
			return (true);
	}
	return (false);
}

bool	Channel::isInvited(std::string nickname) {
	for (std::vector<std::string>::iterator it = _invited.begin(); it != _invited.end(); ++it) {
		if (*it == nickname)
			return (true);
	}
	return (false);
}

void	Channel::addInvited(Client client) {
	_invited.push_back(client.getNickname());
}

void	Channel::addInvited(std::string nickname) {
	_invited.push_back(nickname);
}

void	Channel::removeInvited(Client client) {
	for (std::vector<std::string>::iterator it = _invited.begin(); it != _invited.end(); ++it) {
		if (*it == client.getNickname()) {
			_invited.erase(it);
			return;
		}
	}
}

void	Channel::removeInvited(std::string nickname) {
	for (std::vector<std::string>::iterator it = _invited.begin(); it != _invited.end(); ++it) {
		if (*it == nickname) {
			_invited.erase(it);
			return;
		}
	}
}

std::vector<Client>::iterator	Channel::findClient(Client client) {
	for (std::vector<Client>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
		if (it->getNickname() == client.getNickname())
			return (it);
	}
	return (_clients.end());
}

std::vector<Client>::iterator	Channel::findClient(std::string nickname) {
	for (std::vector<Client>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
		if (it->getNickname() == nickname)
			return (it);
	}
	return (_clients.end());
}

std::vector<Client>::iterator	Channel::findOperator(Client client) {
	for (std::vector<Client>::iterator it = _operators.begin(); it != _operators.end(); ++it) {
		if (it->getNickname() == client.getNickname())
			return (it);
	}
	return (_operators.end());
}

std::vector<Client>::iterator	Channel::findOperator(std::string nickname) {
	for (std::vector<Client>::iterator it = _operators.begin(); it != _operators.end(); ++it) {
		if (it->getNickname() == nickname)
			return (it);
	}
	return (_operators.end());
}

/* REFERENCES:
	* https://se.inf.ethz.ch/student_projects/fabian_gremper/Report.pdf
	*/