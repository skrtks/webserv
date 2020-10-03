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
	int socketFd, connectionFd, fdMax, bytesReceived;
	struct sockaddr_in server = {}; // Will contain info about port and ip
	struct sockaddr_storage their_addr = {}; // Will contain info about connecting client
	socklen_t addr_size;
	fd_set master;    // master file descriptor list
	fd_set readFds;  // temp file descriptor list for select()

	// Make sure these are empty at start
	FD_ZERO(&master);
	FD_ZERO(&readFds);

	if (!(socketFd = socket(AF_INET, SOCK_STREAM, 0)))
		throw std::runtime_error("Failed to create socket");

	// Fill struct with info about port and ip
	server.sin_family = AF_INET; // ipv4
	server.sin_addr.s_addr = INADDR_ANY; // choose local ip for me
	server.sin_port = htons(PORT); // set port (htons translates host bit order to network order)

	// Associate the socket with a port and ip
	if (bind(socketFd, (struct sockaddr *)&server, sizeof(server)) < 0) {
		throw std::runtime_error("Failed to bind");
	}

	// Start listening for connections on port set in sFd, mac BACKLOG waiting connections
	if (listen(socketFd, BACKLOG))
		throw std::runtime_error("Error during listen");

	// Add the listening socket to the master list
	FD_SET(socketFd, &master);

	// Keep track of the biggest fd on the list
	fdMax = socketFd;

	while (true) {
		readFds = master;
		if (select(fdMax+1, &readFds, NULL, NULL, NULL) == -1)
			throw std::runtime_error("Error in select call");
		// Go through existing connections looking for data to read
		for (int i = 0; i <= fdMax; i++) {
			if (FD_ISSET(i, &readFds)) { // Returns true if fd is active
				if (i == socketFd) { // This means there is a new connection waiting to be accepted
					// Accept one connection from backlog
					addr_size = sizeof their_addr;
					if ((connectionFd = accept(socketFd, (struct sockaddr*)&their_addr, &addr_size)) < 0)
						throw std::runtime_error("Error accepting connection");
					FD_SET(connectionFd, &master); // Add new connection to the set
					if (connectionFd > fdMax)
						fdMax = connectionFd;
					std::cout << "New client connected" << std::endl;
				}
				else { // Handle request & return response
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

					close(i); // bye!
					FD_CLR(i, &master);
					request.clear();
				}
			}
		}
	}

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
	// TODO: restart server in proper way after exception
	return 0;
}
