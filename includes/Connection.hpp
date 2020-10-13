/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Connection.hpp                                     :+:    :+:            */
/*                                                     +:+                    */
/*   By: sam <sam@student.codam.nl>                   +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/10/03 15:26:41 by sam           #+#    #+#                 */
/*   Updated: 2020/10/03 15:26:41 by sam           ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include <arpa/inet.h>
#include "RequestParser.hpp"
#include "Server.hpp"

#define BACKLOG 5
#define BUFLEN 8192

class Connection {
	int _socketFd, _connectionFd, _fdMax;
	struct sockaddr_in _serverAddr; // Will contain info about port and ip
	fd_set _master;    // master file descriptor list
	fd_set _readFds;  // temp file descriptor list for select()
	std::string _rawRequest;
	request_s _parsedRequest;
	std::vector<Server> _servers;
	std::map<int, Server> _serverMap; // key: socketFd; value: Corresponding server object
	int addConnection();
	void receiveRequest();
	void sendReply(const std::string &msg) const;
	void closeConnection(int fd);
public:
	Connection();
	Connection(const Connection &obj);
	Connection& operator== (const Connection &obj);
	virtual ~Connection();

	void setUpConnection();
	void startListening();
	void setServers(const std::vector<Server>& servers);
};


#endif //CONNECTION_HPP
