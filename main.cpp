/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   main.cpp                                           :+:    :+:            */
/*                                                     +:+                    */
/*   By: sam <sam@student.codam.nl>                   +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/10/02 16:00:45 by sam           #+#    #+#                 */
/*   Updated: 2020/10/02 16:00:45 by sam           ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <stdexcept>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <zconf.h>

#define PORT 8080	// The port we will be listening on TODO: set through config file
#define BACKLOG 5	// how many pending connections queue will hold
#define BUFLEN 1024

void setupConnection() {
	std::string request;
	char buf[BUFLEN];
	int socketFd, connectionFd, bytesReceived;
	struct sockaddr_in server = {}; // Will contain info about port and ip
	struct sockaddr_storage their_addr = {}; // Will contain info about connecting client
	socklen_t addr_size;

	if (!(socketFd = socket(AF_INET, SOCK_STREAM, 0)))
		throw std::runtime_error("Failed to create socket");

	// Fill struct with info about port and ip
	server.sin_family = AF_INET; // ipv4
	server.sin_addr.s_addr = INADDR_ANY; // choose local ip for me
	server.sin_port = htons(PORT); // set port (htons translates host bit order to network order)

	// Associate the socket with a port and ip
	if (bind(socketFd, (struct sockaddr *)&server, sizeof(server)) < 0) {
		std::cout << strerror(errno) << std::endl;
		throw std::runtime_error("Failed to bind");
	}

	// Start listening for connections on port set in sFd, mac BACKLOG waiting connections
	if (listen(socketFd, BACKLOG))
		throw std::runtime_error("Error during listen");

	// Accept one connection from backlog
	addr_size = sizeof their_addr;
	if ((connectionFd = accept(socketFd, (struct sockaddr*)&their_addr, &addr_size)) < 0)
		throw std::runtime_error("Error accepting connection");

	// Loop to receive complete request, even if buffer is smaller
	// TODO: instead of looping only on bytesReceived, add timeout?
	do {
		bytesReceived = recv(connectionFd, buf, BUFLEN - 1, 0);
		if (bytesReceived == -1)
			throw std::runtime_error("Error receiving request");
		request.append(buf);
		memset(buf, 0, BUFLEN); // TODO: make this ft_memset
	} while (bytesReceived > 0);
	std::cout << "REQUEST: \n" << request;

	// TODO: process request

	// TODO: send proper reply
	char msg[] = "Thank you for your request, in the future you'll get a proper reply\n";
	if ((send(connectionFd, msg, 68, 0) == -1))
		throw std::runtime_error("Error sending reply");

	close(socketFd);
	close(connectionFd);
}

int main() {
	try {
		setupConnection();
	}
	catch (std::runtime_error &e) {
		std::cout << e.what() << std::endl;
	}

	return 0;
}
