/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   irc.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lprates <lprates@student.42lisboa.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/18 15:17:37 by lprates           #+#    #+#             */
/*   Updated: 2023/03/25 18:50:04 by lprates          ###   ########.fr       */
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

# include "Client.hpp"

# define MAX_CONNECTIONS 500
// have to check if 1024 or 1025
# define BUFFER_SIZE 1025
# define MAX_CLIENTS 100
# define TRUE   1 
# define FALSE  0 

/*struct Client {
	int socket; // client socket file descriptor
	std::string input_buffer; // input buffer for receiving data from the client
	std::string output_buffer; // output buffer for sending data to the client
	bool is_registered; // connection state, true if the client is registered with a nickname
	std::string nickname; // the client's nickname
	// add more fields as needed...
};*/

#endif