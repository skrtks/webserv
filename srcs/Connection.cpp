/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Connection.cpp                                     :+:    :+:            */
/*                                                     +:+                    */
/*   By: sam <sam@student.codam.nl>                   +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/10/03 15:26:44 by sam           #+#    #+#                 */
/*   Updated: 2020/12/15 13:13:50 by tuperera      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Connection.hpp"
#include <iostream>
#include <zconf.h>
#include <Colours.hpp>
#include "ResponseHandler.hpp"
#include "libftGnl.hpp"

Connection::Connection() : _socketFd(), _readFds(), _writeFds(), _readFdsBak(), _writeFdsBak() {
	FD_ZERO(&_readFds);
	FD_ZERO(&_writeFds);
	FD_ZERO(&_readFdsBak);
	FD_ZERO(&_writeFdsBak);
	_configPath = NULL;
}

Connection::Connection(char *configPath) : _socketFd(), _readFds(), _writeFds(), _readFdsBak(), _writeFdsBak() {
	FD_ZERO(&_readFds);
	FD_ZERO(&_writeFds);
	FD_ZERO(&_readFdsBak);
	FD_ZERO(&_writeFdsBak);
	_configPath = configPath;
}

Connection::~Connection() {
	this->stopServer();
}

Connection::Connection(const Connection &obj) : _socketFd(), _readFds(), _writeFds(), _readFdsBak(), _writeFdsBak(), _configPath() {
	*this = obj;
}

Connection& Connection::operator= (const Connection &obj) {
	if (this != &obj) {
		this->_socketFd = obj._socketFd;
		this->_readFds = obj._readFds;
		this->_writeFds = obj._writeFds;
		this->_readFdsBak = obj._readFdsBak;
		this->_writeFdsBak = obj._writeFdsBak;
		this->_parsedRequest = obj._parsedRequest;
		this->_servers = obj._servers;
		this->_configPath = obj._configPath;
		this->_allConnections = obj._allConnections;
	}
	return *this;
}

void Connection::startListening() {
	RequestParser					requestParser;
	ResponseHandler					responseHandler;
	std::string						response;

	std::cout << "Waiting for connections..." << std::endl;
	while (true) {
		_readFds = _readFdsBak;
		_writeFds = _writeFdsBak;
//		std::cerr << _PURPLE "Before select( ), maxfd is " << this->getMaxFD() << _END << std::endl;
		int selectret = select(this->getMaxFD(), &_readFds, &_writeFds, 0, 0);
		if (selectret == -1)
			throw std::runtime_error(strerror(errno));
		std::cerr << _GREEN "After select call, returned " << selectret << _END << std::endl;
		// Go through existing connections looking for data to read
		for (std::vector<Server*>::iterator it = _servers.begin(); it != _servers.end(); ++it) {
			Server*	s = *it;
			if (FD_ISSET(s->getSocketFd(), &_readFds)) {
				int clientfd = s->addConnection();
				this->_allConnections.insert(clientfd);
				FD_SET(clientfd, &_readFdsBak);
			}
			for (std::vector<Client*>::iterator itc = s->_connections.begin(); !s->_connections.empty() && itc != s->_connections.end(); itc++) {
				Client* c = *itc;
				if (FD_ISSET(c->fd, &_readFds)) {
					c->receiveRequest();
					FD_CLR(c->fd, &_readFdsBak);
					FD_SET(c->fd, &_writeFdsBak);
				}
				if (FD_ISSET(c->fd, &_writeFds)) {
					if (checkIfEnded(c->req) || c->receiveRequest() == 0) {
						_parsedRequest = requestParser.parseRequest(c->req);
						_parsedRequest.server = c->parent;
						response = responseHandler.handleRequest(_parsedRequest);
						sendReply(response.c_str(), c->fd, _parsedRequest);
						response.clear();
						FD_CLR(c->fd, &_writeFdsBak);
						FD_CLR(c->fd, &_readFdsBak);
						this->_allConnections.erase(c->fd);
						delete *itc;
						itc = s->_connections.erase(itc);
						if (itc == s->_connections.end())
							break;
					}
				}
			}
		}
	}
}

void Connection::sendReply(const char* msg, const int& fd, request_s& request) const {
	long	bytesToSend = ft_strlen(msg),
			bytesSent(0),
			sendRet;
	while (bytesToSend > 0) {
		sendRet = send(fd, msg + bytesSent, bytesToSend, 0);
		if (sendRet == -1) {
			if (bytesToSend == 0 || errno == EWOULDBLOCK)
				continue;
			throw (std::runtime_error(strerror(errno)));
		}
		bytesSent += sendRet;
		bytesToSend -= sendRet;
	}
	static int i = 0, post = 0;
	std::cerr << _PURPLE "sent response for request #" << i++ << " (" << methodAsString(request.method);
	if (request.method == POST)
		std::cerr << " #" << post++;
	std::cerr << ").\n" _END;
}

// ------------------ Process Handling ------------------------
void Connection::startServer() {
	loadConfiguration();
	startListening();
}

void Connection::stopServer() {
	// Go through existing connections and close them
	std::cout << "stopping server br\n";
	std::vector<Server*>::iterator	sit;
	std::vector<Client*>::iterator	cit;
	for (sit = _servers.begin(); sit != _servers.end(); ++sit) {
		for (cit = (*sit)->_connections.begin(); cit != (*sit)->_connections.end(); ++cit) {
			delete *cit;
		}
		(*sit)->_connections.clear();
		delete *sit;
		sit = _servers.erase(sit);
	}
	_servers.clear();
//	ft_memset(&_serverAddr, 0, sizeof(_serverAddr)); // Clear struct from prev setup
	FD_ZERO(&_readFds);
	FD_ZERO(&_writeFds);
}

void Connection::loadConfiguration() {
	this->_servers = parse(_configPath);
	for (std::vector<Server*>::const_iterator it = _servers.begin(); it != _servers.end(); ++it) {
		std::cout << *(*it);
		(*it)->startListening();
		FD_SET((*it)->getSocketFd(), &_readFdsBak);
		this->_allConnections.insert((*it)->getSocketFd());
	}
}

void Connection::handleCLI(const std::string& input) {
	if (input == "exit") {
		std::cout << "Shutting down..." << std::endl;
		stopServer();
		exit(0);
	}
	else if (input == "restart") {
		std::cout << "Restarting server..." << std::endl;
		stopServer();
		startServer();
	}
	else if (input == "help") {
		std::cout << "Please use one of these commands:\n"
					 "\n"
					 "   help              For this help\n"
					 "   exit              Shut down the server\n"
					 "   restart           Restart the server\n" << std::endl;
	}
	else {
		std::cout << "Command \"" << input << "\" not found. Type \"help\" for available commands" << std::endl;
	}
}

bool Connection::checkIfEnded(const std::string& request) {
	size_t chunkedPos;
	size_t encoPos = request.find("Transfer-Encoding:");
	if (encoPos != std::string::npos) {
		chunkedPos = request.find("chunked", encoPos);
	}
	if (encoPos != std::string::npos && chunkedPos != std::string::npos) {
		size_t endSequencePos = request.find("\r\n0\r\n\r\n");
		size_t len = request.length();
		if (endSequencePos != std::string::npos && endSequencePos + 7 == len) {
			return true;
		}
	}
	else {
		size_t endSequencePos = request.find_last_not_of("\r\n\r\n");
		size_t len = request.length();
		if (endSequencePos != std::string::npos && endSequencePos + 5 == len) {
			return true;
		}
	}
	return false;
}

int Connection::getMaxFD() {
	return (*std::max_element(this->_allConnections.begin(), this->_allConnections.end()) + 1);
}
