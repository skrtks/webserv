//
// Created by peerdb on 13-01-21.
//

#ifndef WEBSERV_CLIENT_HPP
#define WEBSERV_CLIENT_HPP
#include	<arpa/inet.h>
#include	<string.h>
#include	<iostream>
#include	"RequestParser.hpp"
#include	"Colours.hpp"

# ifndef CONNECTION_LOGS
#  define CONNECTION_LOGS 0
# endif
#define	BUFLEN 8192

class Server;
struct Client {
	Server* parent;
	int		fd,
			port;
	bool	open;
	struct sockaddr_in addr;
	socklen_t size;
	std::string req,
				host;
	std::string ipaddress;
	time_t	lastRequest;
	request_s	parsedRequest;

	explicit Client(Server* x);
	~Client();
	int		receiveRequest();
	void	resetTimeout();
	void 	sendReply(const char* msg, request_s& request) const;
	void	checkTimeout();
	void	reset(const std::string&);

private:
	Client();
};

#endif //WEBSERV_CLIENT_HPP
