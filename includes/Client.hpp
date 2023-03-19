#ifndef CLASS_HPP
# define CLASS_HPP

# include <string>

class Client
{
	public:
		explicit			Client(int socketFd) : _socketFd(socketFd), _isRegistered(false) {}
		
		int					getSocketFd() const { return (_socketFd); }
		std::string&		getInputBuffer() { return (_inputBuffer); }
		std::string&		getOutputBuffer() { return (_outputBuffer); }
		bool				isRegistered() const { return (_isRegistered); }
		void				setRegistered(bool registered) { _isRegistered = registered; }
		const std::string&	getNickname() const { return (_nickname); }
		void				setNickname(const std::string& nickname) { _nickname = nickname; }

	private:
		int			_socketFd;
		std::string	_inputBuffer;
		std::string	_outputBuffer;
		bool		_isRegistered;
		std::string	_nickname;
};

#endif