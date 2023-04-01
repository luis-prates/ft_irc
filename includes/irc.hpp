/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   irc.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lprates <lprates@student.42lisboa.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/18 15:17:37 by lprates           #+#    #+#             */
/*   Updated: 2023/04/01 11:21:32 by lprates          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRC_HPP
# define IRC_HPP

# include <string>
# include <iostream>
# include <map>
# include <vector>
# include <sys/types.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <fcntl.h>
# include <cstdlib>
# include <unistd.h>
# include <sys/time.h>
# include <cerrno>
# include <sys/epoll.h>
# include <cstdio>
# include <cstring>
# include <csignal>
# include <bits/stdc++.h>

# include "Client.hpp"
# include "Server.hpp"

# define MAX_CONNECTIONS 500
// have to check if 1024 or 1025
# define BUFFER_SIZE 1025
# define MAX_CLIENTS 100
# define TRUE   1 
# define FALSE  0 

typedef struct	s_server
{
	int					port;
	int					socket;
	int					fd;
	int					addrlen;
	struct sockaddr_in	address;
}				t_server;

// utils.cpp
std::vector<std::string>	split(std::string message, char del);
void	handle_signal(int signal);

// input.cpp
int	handle_user_input(std::string message, Client *client);
int	setup_server(t_server *server, int portNumber);

#endif