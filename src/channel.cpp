	#include "../includes/Channel.hpp"
	#include "../includes/Server.hpp"
	
	void Channel::addClient(Client client) {
		_clients.push_back(client);
	}

	void Channel::addOperator(Client op) {
		_operators.push_back(op);
	}

	void Channel::removeClient(Client client)	{
		for (std::vector<Client>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
			if (it->getNickname() == client.getNickname()) {
				_clients.erase(it);
				break;
			}
		}
	}

	void Channel::removeOp(Client op)	{
		for (std::vector<Client>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
			if (it->getNickname() == op.getNickname()) {
				_clients.erase(it);
				break;
			}
			{
				_clients.erase(it);
				break;
			}
		}
	}

	void Channel::joinChannel(Client client) {
		Channel::addClient(client);
		Channel::addOperator(client);
	}

	std::string Channel::getTopic() { return (_topic); }

	void Channel::setTopic(std::string topic) { _topic = topic; }

	std::string Channel::getName() { return (_name); }

	/* REFERENCES:
	 * https://se.inf.ethz.ch/student_projects/fabian_gremper/Report.pdf
	 */