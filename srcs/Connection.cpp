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
#include <signal.h>
#include <cerrno>
#include <algorithm>
#include "Enums.hpp"

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
		this->_servers = obj._servers;
		this->_configPath = obj._configPath;
		this->_allConnections = obj._allConnections;
	}
	return *this;
}

void Connection::startListening() {
	std::cout << "Waiting for connections..." << std::endl;
	while (true) {
		_readFds = _readFdsBak;
		_writeFds = _writeFdsBak;
//		this->_servers.front()->showclients(_readFds, _writeFds);
		if (select(this->getMaxFD(), &_readFds, &_writeFds, 0, 0) == -1)
			throw std::runtime_error(strerror(errno));
		if (FD_ISSET(0, &_readFds))
			this->handleCLI();
		// Go through existing connections looking for data to read
		for (std::vector<Server*>::iterator it = _servers.begin(); it != _servers.end(); ++it) {
			Server*	s = *it;
			if (FD_ISSET(s->getSocketFd(), &_readFds)) {
				int clientfd = s->addConnection();
				this->_allConnections.insert(clientfd);
				FD_SET(clientfd, &_readFdsBak);
			}
			std::vector<Client*>::iterator itc = s->_connections.begin();
			while (itc != s->_connections.end()) {
				Client* c = *itc;
				if (FD_ISSET(c->fd, &_readFds)) {
					if (c->receiveRequest() == 1 && checkIfEnded(c->req)) {
						FD_SET(c->fd, &_writeFdsBak);
					}
				}
				if (FD_ISSET(c->fd, &_writeFds)) {
					RequestParser					requestParser;
					ResponseHandler					responseHandler;
					std::string						response;

					c->parsedRequest = requestParser.parseRequest(c->req);
					c->parsedRequest.server = c->parent;
					std::cerr << "requestparser status code is " << requestParser._status_code << std::endl;
					if (requestParser._status_code != 400) {
						response = responseHandler.handleRequest(c->parsedRequest);
						c->sendReply(response.c_str(), c->parsedRequest);
					}
					response.clear();
					c->reset(responseHandler._header_vals[CONNECTION]);
					FD_CLR(c->fd, &_writeFdsBak);
				}
				c->checkTimeout();
				if (!c->open) {
					std::cerr << c->fd << " at " << c->ipaddress << " is closing\n";
					FD_CLR(c->fd, &_readFdsBak);
					FD_CLR(c->fd, &_writeFdsBak);
					this->_allConnections.erase(c->fd);
					delete *itc;
					s->_connections.erase(itc);
					if (s->_connections.empty())
						break;
					else {
						itc = s->_connections.begin();
						continue;
					}
				}
				++itc;
			}
		}
	}
}

// ------------------ Process Handling ------------------------
Connection*	THIS;
void Connection::startServer() {
	loadConfiguration();
	THIS = this;
	signal(SIGINT, Connection::signalServer);
	startListening();
}

void Connection::signalServer(int n) {
	THIS->stopServer();
	exit(n);
}

void Connection::stopServer() {
	// Go through existing connections and close them
	std::vector<Server*>::iterator	sit;
	std::vector<Client*>::iterator	cit;
	for (sit = _servers.begin(); sit != _servers.end(); ++sit) {
		for (cit = (*sit)->_connections.begin(); cit != (*sit)->_connections.end(); ++cit) {
			delete *cit;
		}
		(*sit)->_connections.clear();
		delete *sit;
		sit = _servers.erase(sit);
		if (_servers.empty())
			break;
	}
	_servers.clear();
	FD_ZERO(&_readFds);
	FD_ZERO(&_writeFds);
	FD_ZERO(&_readFdsBak);
	FD_ZERO(&_writeFdsBak);
	std::cerr << _GREEN "\nServer stopped gracefully.\n" << _END;
}

void Connection::loadConfiguration() {
	FD_SET(0, &_readFdsBak);
	this->_servers = parse(_configPath);
	for (std::vector<Server*>::const_iterator it = _servers.begin(); it != _servers.end(); ++it) {
		std::cout << *(*it);
		(*it)->startListening();
		FD_SET((*it)->getSocketFd(), &_readFdsBak);
		this->_allConnections.insert((*it)->getSocketFd());
	}
}

void Connection::handleCLI() {
	std::string input;
	std::getline(std::cin, input);

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
