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

Connection::Connection() : _server(), _master(), _readFds() {
	FD_ZERO(&_master);
	FD_ZERO(&_readFds);
	_socketFd = 0;
	_connectionFd = 0;
	_fdMax = 0;
}

Connection::~Connection() {
	close(_socketFd);
	close(_connectionFd);
}

Connection::Connection(const Connection &obj) {
	*this = obj;
}

Connection& Connection::operator== (const Connection &obj) {
	if (this == &obj) {
		return *this;
	}
	*this = obj;
	return *this;
}

void Connection::setUpConnection() {
	int opt = 1;
	if (!(_socketFd = socket(AF_INET, SOCK_STREAM, 0)))
		throw std::runtime_error(strerror(errno));

	if (setsockopt(_socketFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
		throw std::runtime_error(strerror(errno));
	}

	// Fill struct with info about port and ip
	_server.sin_family = AF_INET; // ipv4
	_server.sin_addr.s_addr = INADDR_ANY; // choose local ip for me
	_server.sin_port = htons(PORT); // set port (htons translates host bit order to network order)

	// Associate the socket with a port and ip
	if (bind(_socketFd, (struct sockaddr *)&_server, sizeof(_server)) < 0) {
		throw std::runtime_error(strerror(errno));
	}
}

void Connection::startListening() {
	RequestParser requestParser;
	RequestHandler requestHandler;
	std::string response;
	// Start listening for connections on port set in sFd, mac BACKLOG waiting connections
	if (listen(_socketFd, BACKLOG))
		throw std::runtime_error(strerror(errno));
	// Add the listening socket to the master list
	FD_SET(_socketFd, &_master);
	// Keep track of the biggest fd on the list
	_fdMax = _socketFd;
	std::cout << "Waiting for connections..." << std::endl;
	while (true) {
		_readFds = _master;
		if (select(_fdMax + 1, &_readFds, NULL, NULL, NULL) == -1)
			throw std::runtime_error(strerror(errno));
		// Go through existing connections looking for data to read
		for (int i = 0; i <= _fdMax; i++) {
			if (FD_ISSET(i, &_readFds)) { // Returns true if fd is active
				if (i == _socketFd) { // This means there is a new connection waiting to be accepted
					addConnection();
				}
				else { // Handle request & return response
					receiveRequest();
					_parsedRequest = requestParser.parseRequest(_rawRequest);
					response = requestHandler.handleRequest(_parsedRequest);
					sendReply(response);
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
	if ((_connectionFd = accept(_socketFd, (struct sockaddr*)&their_addr, &addr_size)) < 0)
		throw std::runtime_error(strerror(errno));
	FD_SET(_connectionFd, &_master); // Add new connection to the set
	if (_connectionFd > _fdMax)
		_fdMax = _connectionFd;
	std::cout << "New client connected" << std::endl;
}

void Connection::receiveRequest() {
	char buf[BUFLEN];
	std::string request;
	int bytesReceived;
	// Loop to receive complete request, even if buffer is smaller
	request.clear();
	memset(buf, 0, BUFLEN); // TODO: make this ft_memset
	do {
		bytesReceived = recv(_connectionFd, buf, BUFLEN - 1, 0);
		if (bytesReceived == -1)
			throw std::runtime_error(strerror(errno));
		request += buf;
		memset(buf, 0, BUFLEN); // TODO: make this ft_memset
	} while (bytesReceived == BUFLEN - 1);
	_rawRequest = request;
//	std::cout << "REQUEST: \n" << request;
}

void Connection::sendReply(const std::string &msg) const {
	// TODO: send proper reply

	if ((send(_connectionFd, msg.c_str(), msg.length(), 0) == -1))
		throw std::runtime_error(strerror(errno));
}

void Connection::closeConnection(int fd) {
	// Closing connection after response has been send
	close(fd);
	FD_CLR(fd, &_master);
}
