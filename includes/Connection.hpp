/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Connection.hpp                                     :+:    :+:            */
/*                                                     +:+                    */
/*   By: sam <sam@student.codam.nl>                   +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/10/03 15:26:41 by sam           #+#    #+#                 */
/*   Updated: 2020/11/07 10:55:00 by tuperera      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include <arpa/inet.h>
#include <cerrno>
#include "RequestParser.hpp"
#include "Server.hpp"
#include "Servermanager.hpp"

#define BACKLOG 5
#define BUFLEN 8192

class Connection {
	int _connectionFd, _fdMax;
	struct sockaddr_in _serverAddr; // Will contain info about port and ip
	fd_set _master;    // master file descriptor list
	fd_set _readFds;  // temp file descriptor list for select()
	std::string _rawRequest;
	request_s _parsedRequest;
	std::vector<Server> _servers;
	std::map<int, Server> _serverMap; // key: socketFd; value: Corresponding server object
	Servermanager _manager;
	char *_configPath;
	int addConnection(const int &socketFd);
	void receiveRequest(const int& fd);
	void sendReply(const std::string& msg, const int& fd) const;
	void closeConnection(const int& fd);
public:
	Connection();
	explicit Connection(char* configPath);
	Connection(const Connection &obj);
	Connection& operator= (const Connection &obj);
	virtual ~Connection();

	void setUpConnection();
	void startListening();
	void setServers(const std::vector<Server>& servers);

	void startServer();
	void loadConfiguration();
	void handleCLI(std::string input);
};

#endif //CONNECTION_HPP
