#ifndef SERVER_HPP
# define SERVER_HPP

# include "../includes/Channel.hpp"
# include "irc.hpp"
# include <map>

# define SERVERNAME "ircserv"

class Server
{
	private:
		int								_port;
		int								_socket;
		int								_fd;
		int								_addrlen;
		int								_maxFd;
		fd_set							_readFds;
		struct sockaddr_in				_address;
		std::string						_hostname;
		std::string						_password;

		std::vector<Channel> 			_channels;
		std::vector<Client>				_clients;

		int								fdResetNSet(std::vector<Client> &clients);
		int								handleNewConnection(std::vector<Client> &clients);
		int								handleClientInput(Client &client);
		int								handleCommands(std::string message, Client &client);
		std::vector<std::string>		split(std::string message, char del);
		int								handleNick(std::vector<std::string> params, Client &client);
		int								handleUser(std::vector<std::string> params, Client &client);
		int								joinChannel(std::vector<std::string> params, Client &client);

		std::vector<Channel>::iterator	getChannel(std::string channelName);
		int								rpl_Join(Client client, Channel &newChannel);
		int								rpl_Welcome(const Client &client);
		int								who(std::vector<std::string> params, Client &client);
		int								privmsg(std::vector<std::string> params, Client &client);
		int								notice(std::vector<std::string> params, Client &client);
		int								part(std::vector<std::string> params, Client &client);
		int								quit(std::vector<std::string> params, Client &client);
		int								kick(std::vector<std::string> params, Client &client);
		int								user(std::vector<std::string> params, Client &client);
		int								pass(std::vector<std::string> params, Client &client);
		int 							mode(std::vector<std::string> params, Client &client);
		int								invite(std::vector<std::string> params, Client &client);
		int								topic(std::vector<std::string> params, Client &client);
		int								invalidCommand(std::string command, Client &client);

		void 							channelMode(std::string channelName, Client &client);
		bool							checkChannel(std::string channelName, Client &client);
		bool							checkChannelExists(std::string channelName);
		int								createNewChannel(std::string channelName, Client &client);
		bool							checkClientExists(std::string nickname);
		std::vector<Client>::iterator	getClientIterator(std::string nickname);
		std::vector<Channel>::iterator	getChannelIterator(std::string channelName);
		void							cleanClientFromServer(Client &client);
		//int							displayUserModes(std::string params, Client &client);
		int								modeUser(std::vector<std::string> params, Client &client);
		int								handleNotRegistered(Client &client);


	public:

		explicit						Server(std::string password);

		~Server();
		int								getPort() const { return (_port); }
		int								getSocket() const { return (_socket); }
		int								getFd() const { return (_fd); }
		int								getAddrlen() const { return (_addrlen); }
		struct sockaddr_in				getAddress() const { return (_address); }
		std::string						getHostname() const { return (_hostname); }

		void							setPort(int port) { _port = port; }
		void							setSocket(int socket) { _socket = socket; }
		void							setFd(int fd) { _fd = fd; }
		void							setAddrlen(int addrlen) { _addrlen = addrlen; }
		void							setAddress(struct sockaddr_in address) { _address = address; }
		void							setHostname(std::string hostname) { _hostname = hostname; }
		void							setClients(std::vector<Client> &clients) { _clients = clients; }

		int								setup_server(int port);
		bool							run();
		int								clear_fd_set();

		class ServerException : public std::exception
		{
			public:
				virtual const char* what() const throw()
				{
					return ("Server Exception");
				}
		};
		
};

typedef int (Server::*commandHandler)(std::vector<std::string>, Client &);

#endif