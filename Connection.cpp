/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Connection.cpp                                     :+:    :+:            */
/*                                                     +:+                    */
/*   By: sam <sam@student.codam.nl>                   +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/10/03 15:26:44 by sam           #+#    #+#                 */
/*   Updated: 2020/10/03 15:26:44 by sam           ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Connection.hpp"

Connection::Connection() {
	FD_ZERO(&master);
	FD_ZERO(&readFds);
	socketFd = 0;
	connectionFd = 0;
	fdMax = 0;
}

Connection::~Connection() {
	close(socketFd);
	close(connectionFd);
}

void Connection::setUpConnection() {
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
}

void Connection::startListening() {
	// Start listening for connections on port set in sFd, mac BACKLOG waiting connections
	if (listen(socketFd, BACKLOG))
		throw std::runtime_error("Error during listen");
	// Add the listening socket to the master list
	FD_SET(socketFd, &master);
	// Keep track of the biggest fd on the list
	fdMax = socketFd;
	std::cout << "Waiting for connections..." << std::endl;
	while (true) {
		readFds = master;
		if (select(fdMax + 1, &readFds, NULL, NULL, NULL) == -1)
			throw std::runtime_error("Error in select call");
		// Go through existing connections looking for data to read
		for (int i = 0; i <= fdMax; i++) {
			if (FD_ISSET(i, &readFds)) { // Returns true if fd is active
				if (i == socketFd) { // This means there is a new connection waiting to be accepted
					addConnection();
				}
				else { // Handle request & return response
					receiveRequest();
					// TODO: process request
					std::string msg = "Thank you for your request, in the future you'll get a proper reply\n";
					sentReply(msg);
					closeConnection(i);
				}
			}
		}
	}
}

void Connection::addConnection() {
	struct sockaddr_storage their_addr = {}; // Will contain info about connecting client
	socklen_t addr_size;

	// Accept one connection from backlog
	addr_size = sizeof their_addr;
	if ((connectionFd = accept(socketFd, (struct sockaddr*)&their_addr, &addr_size)) < 0)
		throw std::runtime_error("Error accepting connection");
	FD_SET(connectionFd, &master); // Add new connection to the set
	if (connectionFd > fdMax)
		fdMax = connectionFd;
	std::cout << "New client connected" << std::endl;
}

void Connection::receiveRequest() const {
	char buf[BUFLEN];
	std::string request;
	int bytesReceived;
	// Loop to receive complete request, even if buffer is smaller
	// TODO: instead of looping only on bytesReceived, add timeout?
	request.clear();
	memset(buf, 0, BUFLEN); // TODO: make this ft_memset
	do {
		bytesReceived = recv(connectionFd, buf, BUFLEN - 1, 0);
		if (bytesReceived == -1)
			throw std::runtime_error("Error receiving request");
		request.append(buf);
		memset(buf, 0, BUFLEN); // TODO: make this ft_memset
	} while (bytesReceived > 0);
	std::cout << "REQUEST: \n" << request;
}

void Connection::sentReply(const std::string &msg) const {
	// TODO: send proper reply

	if ((send(connectionFd, msg.c_str(), 68, 0) == -1))
		throw std::runtime_error("Error sending reply");
}

void Connection::closeConnection(int fd) {
	// Closing connection after response has been send
	close(fd);
	FD_CLR(fd, &master);
}