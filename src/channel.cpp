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

bool	Channel::isClientInChannel(Client client) {
	for (std::vector<Client>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
		if (it->getNickname() == client.getNickname())
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

/* REFERENCES:
	* https://se.inf.ethz.ch/student_projects/fabian_gremper/Report.pdf
	*/