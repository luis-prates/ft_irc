#include "../includes/Client.hpp"

void	Client::clearClient() {
	_isRegistered = false;
	_nickname.clear();
	_username.clear();
	_realname.clear();
	_isOperator = false;
	_ipAddress = NULL;
	_port = 0; 
	_socketFd = 0;
	_inputBuffer.clear();
	_outputBuffer.clear();
}