//
// Created by peerdb on 13-01-21.
//

#include "Client.hpp"
#include "Server.hpp"
#include "libftGnl.hpp"
#include <cerrno>

Client::Client(Server* S) : parent(S), fd(), port(), open(true), addr(), size(sizeof(addr)), lastRequest(0), parsedRequest() {
	bzero(&addr, size);
	this->fd = accept(S->getSocketFd(), (struct sockaddr*)&addr, &size);
	if (this->fd == -1) {
		std::cerr << _RED _BOLD "Error accepting connection\n" _END;
		throw std::runtime_error(strerror(errno));
	}
	if (fcntl(fd, F_SETFL, O_NONBLOCK) == -1) {
		std::cerr << _RED _BOLD "Error setting connection fd to be nonblocking\n" _END;
		throw std::runtime_error(strerror(errno));
	}
	int opt = 1;
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
		std::cerr << _RED _BOLD "Error setting connection fd socket options\n" _END;
		throw std::runtime_error(strerror(errno));
	}
	this->host = inet_ntoa(addr.sin_addr);
	this->port = htons(addr.sin_port);
	this->ipaddress = host + ':' + ft::inttostring(port);
	if (CONNECTION_LOGS)
		std::cerr << _YELLOW "Opened a new client for " << fd << " at " << ipaddress << std::endl << _END;
}

Client::~Client() {
	close(fd);
	fd = -1;
	req.clear();
	this->parsedRequest.clear();
}

int Client::receiveRequest() {
	char buf[BUFLEN + 1];
	int recvRet;
	bool recvCheck(false);

	// Loop to receive complete request, even if buffer is smaller
	ft_memset(buf, 0, BUFLEN);
	this->resetTimeout();
	while ((recvRet = recv(this->fd, buf, BUFLEN, 0)) > 0) {
		buf[recvRet] = '\0';
		this->req.append(buf);
		recvCheck = true;
	}
	if (!recvCheck or recvRet == 0) { // Not possible to read from the socket (done reading or socket closed)
		this->open = false;
		return (0);
	}
	return (1);
}

void Client::sendReply(const char* msg, request_s& request) const {
	long	bytesToSend = ft_strlen(msg),
			bytesSent(0),
			sendRet;
	while (bytesToSend > 0) {
		sendRet = send(this->fd, msg + bytesSent, bytesToSend, 0);
		if (sendRet == -1) {
			if (bytesToSend != 0)
				continue;
			throw (std::runtime_error(strerror(errno)));
		}
		bytesSent += sendRet;
		bytesToSend -= sendRet;
	}
	static int i = 1, post = 1;
	std::cerr << _PURPLE "sent response for request #" << i++ << " (" << methodAsString(request.method);
	if (request.method == POST)
		std::cerr << " #" << post++;
	std::cerr << ").\n" _END;
}

void Client::resetTimeout() {
	this->lastRequest = ft::getTime();
}

void Client::checkTimeout() {
	if (this->lastRequest) {
		time_t diff = ft::getTime() - this->lastRequest;
//		std::cerr << "timediff is " << diff << std::endl;
		if (diff > 10000000)
			this->open = false;
	}
}

void Client::reset(const std::string& connection) {
	if (connection == "close") {
		if (CONNECTION_LOGS)
			std::cerr << "We ain't resetting, we're closing this client, baby" << std::endl;
		this->open = false;
		return;
	}
	if (CONNECTION_LOGS)
		std::cerr << "Resetting client!\n";
	this->open = true;
	req.clear();
	lastRequest = 0;
	this->parsedRequest.clear();
}

Client::Client() : parent(), fd(), port(), open(), addr(), size(), lastRequest() {

}
