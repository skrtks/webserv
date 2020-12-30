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
#include <fstream> // TODO RM

#define REQUEST_TIMEOUT 100000

Connection::Connection() : _serverAddr(), _master(), _readFds() {
	FD_ZERO(&_master);
	FD_ZERO(&_readFds);
	_connectionFd = 0;
	_fdMax = 0;
	_configPath = NULL;
}

Connection::Connection(char *configPath) : _serverAddr(), _master(), _readFds() {
	FD_ZERO(&_master);
	FD_ZERO(&_readFds);
	_connectionFd = 0;
	_fdMax = 0;
	_configPath = configPath;
}

Connection::~Connection() {
	for (std::vector<Server>::iterator it = _servers.begin(); it != _servers.end(); it++) {
		close(it->getSocketFd());
	}
	close(_connectionFd);
}

Connection::Connection(const Connection &obj) : _connectionFd(), _fdMax(), _serverAddr(), _master(), _readFds(), _configPath() {
	*this = obj;
}

Connection& Connection::operator= (const Connection &obj) {
	if (this != &obj) {
		this->_connectionFd = obj._connectionFd;
		this->_connectionFd = obj._connectionFd;
		this->_fdMax = obj._fdMax;
		this->_serverAddr = obj._serverAddr;
		this->_master = obj._master;
		this->_readFds = obj._readFds;
		this->_requestStorage = obj._requestStorage;
		this->_parsedRequest = obj._parsedRequest;
		this->_servers = obj._servers;
		this->_serverMap = obj._serverMap;
		this->_manager = obj._manager;
		this->_configPath = obj._configPath;
	}
	return *this;
}

void Connection::setUpConnection() {
	int socketFd;
	int opt = 1;
	for (std::vector<Server>::iterator it = _servers.begin(); it != _servers.end(); it++) {
		ft_memset(&_serverAddr, 0, sizeof(_serverAddr)); // Clear struct from prev setup
		if (!(socketFd = socket(AF_INET, SOCK_STREAM, 0)))
			throw std::runtime_error(strerror(errno));

		if (setsockopt(socketFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
			throw std::runtime_error(strerror(errno));
		}

		// Fill struct with info about port and ip
		_serverAddr.sin_family = AF_INET; // ipv4
		if (it->gethost() == "localhost") {
			_serverAddr.sin_addr.s_addr = INADDR_ANY; // choose local ip for me
		}
		else {
			_serverAddr.sin_addr.s_addr = inet_addr(it->gethost().c_str());
		}
		_serverAddr.sin_port = htons(it->getport()); // set port (htons translates host bit order to network order)

		// Associate the socket with a port and ip
		for (int i = 0; i < 6; ++i) {
			if (bind(socketFd, (struct sockaddr*) &_serverAddr, sizeof(_serverAddr)) < 0) {
				std::cerr << "Cannot bind (" << errno << " " << strerror(errno) << ")" << std::endl;
				if (i == 5) throw std::runtime_error(strerror(errno));
			}
			else {
				break;
			}
			usleep(TIMEOUT);
		}

		// Start listening for connections on port set in sFd, max BACKLOG waiting connections
		if (listen(socketFd, BACKLOG))
			throw std::runtime_error(strerror(errno));
		it->setSocketFd(socketFd);
	}
}

void Connection::startListening() {
	RequestParser					requestParser;
	ResponseHandler					responseHandler;
	std::vector<std::string>				response;
	std::map<int, Server>::iterator	serverMapIt;
	std::map<int, Server> 			serverConnections;

	FD_SET(0, &_master); // Add stdin so we always listen to cl commands
	for (std::vector<Server>::iterator it = _servers.begin(); it != _servers.end(); it++) {
		// Add the listening socket to the master list
		FD_SET(it->getSocketFd(), &_master);
		// Keep track of the biggest fd on the list
		_fdMax = it->getSocketFd();
		_serverMap.insert(std::make_pair(it->getSocketFd(), *it)); // Keep track of which socket is for which server obj
	}
	std::cout << "Waiting for connections..." << std::endl;
	while (true) {
		_readFds = _master;
		if (select(_fdMax + 1, &_readFds, &_writeFds, NULL, NULL) == -1)
			throw std::runtime_error(strerror(errno));
		// Go through existing connections looking for data to read
		for (int fd = 0; fd <= _fdMax; fd++) {
			if (FD_ISSET(fd, &_readFds)) { // Returns true if fd is active
				if (fd == 0) {
					std::string clInput;
					std::getline(std::cin, clInput);
					handleCLI(clInput);
				}
				else if ((serverMapIt = _serverMap.find(fd)) != _serverMap.end()) { // This means there is a new connection waiting to be accepted
					serverConnections.insert(std::make_pair(addConnection(serverMapIt->second.getSocketFd()), serverMapIt->second));
					std::cout << _BLUE << "\n vvvvvvvvv CONNECTION OPENED vvvvvvvvv \n" << _END << std::endl;
				}
				else { // Handle request & return response
					if (receiveRequest(fd) == 0) {
						FD_CLR(fd, &_master);
					}
					FD_SET(fd, &_writeFds); // Add new connection to the set
				}
			}
			else if (FD_ISSET(fd, &_writeFds) && fd > STDERR_FILENO) { // Returns true if fd is active
				std::map<int, std::string>::iterator req;
				if ((req = _requestStorage.find(fd)) == _requestStorage.end()) {
					throw std::runtime_error("Error retrieving request from map");
				}
				else if (checkIfEnded(req->second, requestParser) || receiveRequest(fd) == 0) {
					_parsedRequest = requestParser.parseRequest(req->second);

					static int testnumber = 0;
					std::string requestfilename = "/tmp/webserv_request" + ft::inttostring(testnumber) + ".txt";
					std::ofstream requestfile(requestfilename.c_str(), std::ios::out | std::ios::trunc);
					if (requestfile.is_open()) {
						requestfile << req->second;
						requestfile.close();
					}
					_parsedRequest.server = serverConnections[fd];
					response = responseHandler.handleRequest(_parsedRequest);
					sendReply(response, fd, _parsedRequest);
					closeConnection(fd);
					FD_CLR(fd, &_writeFds);
					serverConnections.erase(fd);
					_requestStorage.erase(fd);
					std::cout << _BLUE << "\n ^^^^^^^^^ CONNECTION CLOSED ^^^^^^^^^ \n" << _END << std::endl;
//					testnumber += 1;
				}
			}
		}
	}
}

int Connection::addConnection(const int &socketFd) {
	struct sockaddr_storage their_addr = {}; // Will contain info about connecting client
	socklen_t addr_size;

	// Accept one connection from backlog
	addr_size = sizeof their_addr;
	if ((_connectionFd = accept(socketFd, (struct sockaddr*)&their_addr, &addr_size)) < 0)
		throw std::runtime_error(strerror(errno));
	FD_SET(_connectionFd, &_master); // Add new connection to the set
	if (_connectionFd > _fdMax)
		_fdMax = _connectionFd;
	return _connectionFd;
}

int Connection::receiveRequest(const int& fd) {
	char buf[BUFLEN];
	std::string request;
	int bytesReceived;
	// Loop to receive complete request, even if buffer is smaller
	request.clear();
	ft_memset(buf, 0, BUFLEN);
	do {
		bytesReceived = recv(fd, buf, BUFLEN - 1, MSG_DONTWAIT);
//		std::cout << bytesReceived << std::endl;
		if (bytesReceived > 0) {
			request.append(buf, 0, bytesReceived);
			ft_memset(buf, 0, BUFLEN);
		}
	} while (bytesReceived > 0);

	std::map<int, std::string>::iterator req;
	if ((req = _requestStorage.find(fd)) == _requestStorage.end()) {
		_requestStorage.insert(std::make_pair(fd, request));
	}
	else {
		req->second += request;
	}

	// std::cout << "\n ----------- BEGIN REQUEST ----------- \n" << request << " ----------- END REQUEST ----------- \n" << std::endl;
	return bytesReceived;
//	_rawRequest = request;
}

void Connection::sendReply(std::vector<std::string>& msg, const int& fd, request_s& request) const {
	size_t totalsize = 0;
	std::ofstream responsefile("/tmp/webserv_response.txt", std::ios::out | std::ios::trunc);
	if (responsefile.is_open()) {
//		responsefile << "\nRESPONSE --------" << std::endl;
		for (size_t i = 0; i < msg.size(); i++)
			responsefile << msg[i];
//		responsefile << "\nRESPONSE END ----" << std::endl;
		responsefile.close();
	}
	if (request.transfer_buffer) {
		for (size_t i = 0; i < msg.size(); i++) {
			totalsize += msg[i].length();
			if ((send(fd, msg[i].c_str(), msg[i].length(), 0) == -1))
				throw std::runtime_error(strerror(errno));
		}
	}
	else if ((send(fd, msg[0].c_str(), msg[0].length(), 0) == -1)) {
		throw std::runtime_error(strerror(errno));
	}
	else {
		totalsize += msg[0].length();
		std::cerr << "had to open different else block for this...\n";
	}
//	std::cout << _GREEN << "Response send, first line is: " << msg[0].substr(0, msg[0].find('\n')) << _END << std::endl;
	msg.clear();
	std::cerr << _GREEN "sent a total size of " << totalsize << ".\n" _END;
}

void Connection::closeConnection(const int& fd) {
	// Closing connection after response has been send
	close(fd);
	FD_CLR(fd, &_master);
}

void Connection::setServers(const std::vector<Server>& servers) {
	_servers = servers;
}

// ------------------ Process Handling ------------------------
void Connection::startServer() {
	loadConfiguration();
	setUpConnection();
	startListening();
}

void Connection::stopServer() {
	// Go through existing connections and close them
	for (int fd = 0; fd <= _fdMax; fd++) {
		if (FD_ISSET(fd, &_readFds) && fd != 0) { // Returns true if fd is active
			closeConnection(fd);
		}
	}
	for (std::vector<Server>::iterator it = _servers.begin(); it != _servers.end(); it++) {
		close(it->getSocketFd());
	}
	_connectionFd = 0;
	_fdMax = 0;
	_servers.clear();
	_serverMap.clear();
	_manager.clear();
	ft_memset(&_serverAddr, 0, sizeof(_serverAddr)); // Clear struct from prev setup
}

void Connection::loadConfiguration() {
	_manager = parse(_configPath);
	for (size_t i = 0; i < _manager.size(); i++)
		std::cout << _manager[i];

	setServers(_manager.getServers());
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

bool Connection::checkIfEnded(const std::string& request, RequestParser& requestParser) { //TODO shouldnt we pass requestparser by reference?
	std::map<headerType, std::string>::iterator encoding;
	request_s									tmpRequest;

	tmpRequest = requestParser.parseHeadersOnly(request);
	encoding = tmpRequest.headers.find(TRANSFER_ENCODING);

	if (encoding != tmpRequest.headers.end() && encoding->second.find("chunked") != std::string::npos) {
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