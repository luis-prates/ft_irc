#ifndef CLASS_HPP
# define CLASS_HPP

# include <cstdlib>
# include <string>

class Client
{
	public:
		explicit			Client(int socketFd) : _socketFd(socketFd), _isRegistered(false) {}
		
		int					getSocketFd() const { return (_socketFd); }
		void				setSocketFd(int socketFd) { _socketFd = socketFd; }
		std::string&		getInputBuffer() { return (_inputBuffer); }
		std::string&		getOutputBuffer() { return (_outputBuffer); }
		bool				isRegistered() const { return (_isRegistered); }
		void				setRegistered(bool registered) { _isRegistered = registered; }
		const std::string&	getNickname() const { return (_nickname); }
		void				setNickname(const std::string& nickname) { _nickname = nickname; }
		const std::string&	getUsername() const { return (_username); }
		void				setUsername(const std::string& username) { _username = username; }
		const std::string&	getChannel() const { return (_channel); }
		void				setChannel(const std::string& channel) { _channel = channel; }
		bool				isOperator() const { return (_isOperator); }
		void				setOperator(bool isOperator) { _isOperator = isOperator; }
		char*				getIpAddress() const { return (_ipAddress); }
		void				setIpAddress(char* ipAddress) { _ipAddress = ipAddress; }
		u_int16_t			getPort() const { return (_port); }
		void				setPort(u_int16_t port) { _port = port; }

	private:
		int					_socketFd;
		std::string	_inputBuffer;
		std::string	_outputBuffer;
		bool				_isRegistered;
		std::string	_nickname;
		std::string	_username;
		std::string	_channel;
		bool		_isOperator;
		char		*_ipAddress;
		u_int16_t	_port;

		std::vector<std::string>	_channels;
};

#endif