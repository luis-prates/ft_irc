	#include "../includes/Channel.hpp"
	
	void Channel::addClient(Client client) {
		_clients.push_back(&client);
	}

	void Channel::addOperator(Client op) {
		_operators.push_back(&op);
	}

	void Channel::removeClient(Client client)	{
		for (std::vector<Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
			if (*it == &client) {
				_clients.erase(it);
				break;
			}
		}
	}

	void Channel::removeOp(Client op)	{
		for (std::vector<Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
			if (*it == &op)
			{
				_clients.erase(it);
				break;
			}
		}
	}

	void Channel::joinChannel(Client client) {
		Channel::addClient(client);
	}

	/* REFERENCES:
	 * https://se.inf.ethz.ch/student_projects/fabian_gremper/Report.pdf
	 */