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

	bool Channel::kickUser(Client victim)
	{
		bool kick = false;
		kick = removeOp(victim);
		if (kick == false)
			kick = removeClient(victim);
		return kick;
	}

	std::string Channel::getTopic() { return (_topic); }

	void Channel::setTopic(std::string topic) { _topic = topic; }

	std::string Channel::getName() { return (_name); }

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