#ifndef CLASS_HPP
# define CLASS_HPP

# include <cstdlib>
# include <string>

class Client
{
	public:
		explicit							Client(int socketFd);
		
		int									getSocketFd() const;
		void								setSocketFd(int socketFd);
		std::string&						getInputBuffer();
		std::string&						getOutputBuffer();
		bool								isRegistered() const;
		void								setRegistered(bool registered);
		const std::string&					getNickname() const;
		void								setNickname(const std::string& nickname);
		const std::string&					getUsername() const;
		void								setUsername(const std::string& username);
		const std::string&					getRealname() const;
		void								setRealname(const std::string& realname);
		bool								isOperator() const { return (_isOperator); }
		void								setOperator(bool isOperator) { _isOperator = isOperator; }
		char*								getIpAddress() const;
		void								setIpAddress(char* ipAddress);
		u_int16_t							getPort() const;
		void								setPort(u_int16_t port);
		void								clearClient();

	private:
		int								_socketFd;
		std::string						_inputBuffer;
		std::string						_outputBuffer;
		bool							_isRegistered;
		std::string						_nickname;
		std::string						_username;
		std::string						_realname;
		bool							_isOperator;
		char							*_ipAddress;
		u_int16_t						_port;

};

#endif