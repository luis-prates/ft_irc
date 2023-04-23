#ifndef CLASS_HPP
# define CLASS_HPP

# include <cstdlib>
# include <string>
# include <vector>

class Client
{
	public:
		explicit							Client(int socketFd);

		bool								operator==(const Client& client) const;
		
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
		std::vector<std::string>&			getModes();
		void								setMode(std::string mode);
		void								unsetMode(std::string mode);
		std::vector<std::string>::iterator	findMode(std::string mode);
		void								clearClient();

	private:
		int								_socketFd;
		std::string						_inputBuffer;
		std::string						_outputBuffer;
		bool							_isRegistered;
		std::string						_nickname;
		std::string						_username;
		std::string						_realname;
		std::vector<std::string>		_modes;
		bool							_isOperator;
		char							*_ipAddress;
		u_int16_t						_port;

};

#endif