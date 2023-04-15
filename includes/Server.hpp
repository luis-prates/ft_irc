#ifndef SERVER_HPP
# define SERVER_HPP

# include "../includes/Channel.hpp"
# include "irc.hpp"

# define SERVERNAME "ircserv"

class Server
{
	public:
		std::vector<Channel> _channels;

	private:
		int					_port;
		int					_socket;
		int					_fd;
		int					_addrlen;
		int					_maxFd;
		fd_set				_readFds;
		struct sockaddr_in	_address;
		std::string			_hostname;

		std::vector<Client>		_clients;

		int							fdResetNSet(std::vector<Client> &clients);
		int							handleNewConnection(std::vector<Client> &clients);
		int							handleClientInput(Client &client);
		int 						handleCommands(std::string message, Client &client);
		std::vector<std::string>	split(std::string message, char del);
		int							handleNick(std::vector<std::string> params, Client &client);
		int							handleUser(std::vector<std::string> params, Client &client);
		int							joinChannel(std::vector<std::string> params, Client &client, std::string &response);

		Channel*					getChannel(std::string channelName);
		void						rpl_Join(Client client, Channel newChannel, std::string response);
		void						who(std::vector<std::string> params, Client &client);
		void						privmsg(std::vector<std::string> params, Client &client);
		void						part(std::vector<std::string> params, Client &client);

		int							checkChannel(std::string channelName, Client &client);
		void 						createNewChannel(std::string channelName, Client &client, std::string response);

	public:

		Server();
		Server(int port);

		~Server();
		int					getPort() const { return (_port); }
		int					getSocket() const { return (_socket); }
		int					getFd() const { return (_fd); }
		int					getAddrlen() const { return (_addrlen); }
		struct sockaddr_in	getAddress() const { return (_address); }
		std::string			getHostname() const { return (_hostname); }

		void				setPort(int port) { _port = port; }
		void				setSocket(int socket) { _socket = socket; }
		void				setFd(int fd) { _fd = fd; }
		void				setAddrlen(int addrlen) { _addrlen = addrlen; }
		void				setAddress(struct sockaddr_in address) { _address = address; }
		void				setHostname(std::string hostname) { _hostname = hostname; }

		int					setup_server(int port);
		int					run(std::vector<Client> &clients);
		int					clear_fd_set();

		class ServerException : public std::exception
		{
			public:
				virtual const char* what() const throw()
				{
					return ("Server Exception");
				}
		};
		
};

#endif