#ifndef SERVER_HPP
# define SERVER_HPP

# include "irc.hpp"

class Server
{
	private:
		int					_port;
		int					_socket;
		int					_fd;
		int					_addrlen;
		int					_maxFd;
		fd_set				_readFds;
		struct sockaddr_in	_address;

		int							fd_reset_n_set(std::vector<Client> &clients);
		int							handle_new_connection(std::vector<Client> &clients);
		int							handle_client_input(Client &client);
		int 						handle_commands(std::string message, Client &client);
		std::vector<std::string>	split(std::string message, char del);

	public:

		Server();
		Server(int port);

		~Server();
		int					getPort() const { return (_port); }
		int					getSocket() const { return (_socket); }
		int					getFd() const { return (_fd); }
		int					getAddrlen() const { return (_addrlen); }
		struct sockaddr_in	getAddress() const { return (_address); }

		void				setPort(int port) { _port = port; }
		void				setSocket(int socket) { _socket = socket; }
		void				setFd(int fd) { _fd = fd; }
		void				setAddrlen(int addrlen) { _addrlen = addrlen; }
		void				setAddress(struct sockaddr_in address) { _address = address; }

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