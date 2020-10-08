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
	int opt = 1;
	if (!(socketFd = socket(AF_INET, SOCK_STREAM, 0)))
		throw std::runtime_error(strerror(errno));

	if (setsockopt(socketFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
		throw std::runtime_error(strerror(errno));
	}

	// Fill struct with info about port and ip
	server.sin_family = AF_INET; // ipv4
	server.sin_addr.s_addr = INADDR_ANY; // choose local ip for me
	server.sin_port = htons(PORT); // set port (htons translates host bit order to network order)

	// Associate the socket with a port and ip
	if (bind(socketFd, (struct sockaddr *)&server, sizeof(server)) < 0) {
		throw std::runtime_error(strerror(errno));
	}
}

void Connection::startListening() {
	ParseRequest requestProcessor;
	ExecuteHeaders executeHeaders;
	// Start listening for connections on port set in sFd, mac BACKLOG waiting connections
	if (listen(socketFd, BACKLOG))
		throw std::runtime_error(strerror(errno));
	// Add the listening socket to the master list
	FD_SET(socketFd, &master);
	// Keep track of the biggest fd on the list
	fdMax = socketFd;
	std::cout << "Waiting for connections..." << std::endl;
	while (true) {
		readFds = master;
		if (select(fdMax + 1, &readFds, NULL, NULL, NULL) == -1)
			throw std::runtime_error(strerror(errno));
		// Go through existing connections looking for data to read
		for (int i = 0; i <= fdMax; i++) {
			if (FD_ISSET(i, &readFds)) { // Returns true if fd is active
				if (i == socketFd) { // This means there is a new connection waiting to be accepted
					addConnection();
				}
				else { // Handle request & return response
					receiveRequest();
					_parsedRequest = requestProcessor.parseRequest(_rawRequest);
					executeHeaders.startExecution(_parsedRequest);
					// TODO: execute headers
					// TODO: generate response
					std::string resp = "HTTP/1.1 200 OK\n"
									   "Server: Webserv/0.1\n"
									   "Date: Tue, 06 Oct 2020 08:35:16 GMT\n"
									   "Content-Type: text/html\n"
									   "Content-Length: 612\n"
									   "Last-Modified: Tue, 11 Aug 2020 14:52:34 GMT\n"
									   "Connection: keep-alive\n"
									   "ETag: \"5f32b0b2-264\"\n"
									   "Accept-Ranges: bytes\n\n";
					int fd = open("/usr/local/var/www/index.html", O_RDONLY);
					if (fd == -1)
						throw std::runtime_error(strerror(errno));
					int ret;
					char buf[1024];
					do {
						ret = read(fd, buf, 1024);
						resp += buf;
						memset(buf, 0, 1024);
					} while (ret > 0);

					sendReply(resp);
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
		throw std::runtime_error(strerror(errno));
	FD_SET(connectionFd, &master); // Add new connection to the set
	if (connectionFd > fdMax)
		fdMax = connectionFd;
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
		bytesReceived = recv(connectionFd, buf, BUFLEN - 1, 0);
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

	if ((send(connectionFd, msg.c_str(), msg.length(), 0) == -1))
		throw std::runtime_error(strerror(errno));
}

void Connection::closeConnection(int fd) {
	// Closing connection after response has been send
	close(fd);
	FD_CLR(fd, &master);
}
