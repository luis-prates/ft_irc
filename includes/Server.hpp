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
		struct sockaddr_in	_address;

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