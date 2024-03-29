#include "../includes/Client.hpp"

Client::Client(int socketFd) : _socketFd(socketFd), _isRegistered(false)
{}

bool Client::operator==(const Client& client) const
{
	return _nickname == client._nickname;
}

int Client::getSocketFd() const
{
	return (_socketFd);
}

void Client::setSocketFd(int socketFd)
{
	_socketFd = socketFd;
}

std::string& Client::getInputBuffer()
{
	return (_inputBuffer);
}

std::string& Client::getOutputBuffer()
{
	return (_outputBuffer);
}

bool Client::isRegistered() const
{
	return (_isRegistered);
}

void Client::setRegistered(bool registered)
{
	_isRegistered = registered;
}

const std::string& Client::getNickname() const
{
	return (_nickname);
}

void Client::setNickname(const std::string& nickname)
{
	_nickname = nickname;
}

const std::string& Client::getUsername() const
{
	return (_username);
}

void Client::setUsername(const std::string& username)
{
	_username = username;
}

const std::string& Client::getRealname() const
{
	return (_realname);
}

void Client::setRealname(const std::string& realname)
{
	_realname = realname;
}

char* Client::getIpAddress() const
{
	return (_ipAddress);
}

void Client::setIpAddress(char* ipAddress)
{
	_ipAddress = ipAddress;
}

u_int16_t Client::getPort() const
{
	return (_port);
}

void Client::setPort(u_int16_t port)
{
	_port = port;
}

std::vector<std::string>& Client::getModes()
{
	return (_modes);
}

void Client::setMode(std::string mode)
{
	_modes.push_back(mode);
}

void Client::unsetMode(std::string mode)
{
	std::vector<std::string>::iterator it = findMode(mode);
	_modes.erase(it);
}

std::vector<std::string>::iterator Client::findMode(std::string mode)
{
	for (std::vector<std::string>::iterator it = _modes.begin(); it != _modes.end(); it++)
	{
		if (*it == mode)
			return (it);
	}
	return (_modes.end());
}

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