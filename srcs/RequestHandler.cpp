/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   RequestHandler.cpp                                 :+:    :+:            */
/*                                                     +:+                    */
/*   By: skorteka <skorteka@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/10/08 16:15:11 by skorteka      #+#    #+#                 */
/*   Updated: 2020/10/15 21:33:24 by pde-bakk      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "RequestHandler.hpp"
#include "libftGnl.hpp"
#include <iostream>
#include <fcntl.h>
#include <zconf.h>
#include "Colours.hpp"

RequestHandler::RequestHandler() {
	_functionMap[ACCEPT_CHARSET] = &RequestHandler::handleACCEPT_CHARSET;
	_functionMap[ACCEPT_LANGUAGE] = &RequestHandler::handleACCEPT_LANGUAGE;
	_functionMap[ALLOW] = &RequestHandler::handleALLOW;
	_functionMap[AUTHORIZATION] = &RequestHandler::handleAUTHORIZATION;
	_functionMap[CONTENT_LANGUAGE] = &RequestHandler::handleCONTENT_LANGUAGE;
	_functionMap[CONTENT_LENGTH] = &RequestHandler::handleCONTENT_LENGTH;
	_functionMap[CONTENT_LOCATION] = &RequestHandler::handleCONTENT_LOCATION;
	_functionMap[CONTENT_TYPE] = &RequestHandler::handleCONTENT_TYPE;
	_functionMap[DATE] = &RequestHandler::handleDATE;
	_functionMap[HOST] = &RequestHandler::handleHOST;
	_functionMap[LAST_MODIFIED] = &RequestHandler::handleLAST_MODIFIED;
	_functionMap[LOCATION] = &RequestHandler::handleLOCATION;
	_functionMap[REFERER] = &RequestHandler::handleREFERER;
	_functionMap[RETRY_AFTER] = &RequestHandler::handleRETRY_AFTER;
	_functionMap[SERVER] = &RequestHandler::handleSERVER;
	_functionMap[TRANSFER_ENCODING] = &RequestHandler::handleTRANSFER_ENCODING;
	_functionMap[USER_AGENT] = &RequestHandler::handleUSER_AGENT;
	_functionMap[WWW_AUTHENTICATE] = &RequestHandler::handleWWW_AUTHENTICAT;
}

RequestHandler::~RequestHandler() {
}

RequestHandler::RequestHandler(const RequestHandler &obj) {
	this->_functionMap = obj._functionMap;
	this->_response = obj._response;
}

RequestHandler& RequestHandler::operator= (const RequestHandler &obj) {
	if (this != &obj) {
		this->_functionMap = obj._functionMap;
		this->_response = obj._response;
	}
	return *this;
}

std::string RequestHandler::handleRequest(request_s request) {
	std::cout << "Server for this request is: " << request.server.getservername() << std::endl; // todo: remove this for production
//	for (std::map<headerType, std::string>::iterator it=request.headers.begin(); it!=request.headers.end(); it++) {
//		(this->*(_functionMap.at(it->first)))(it->second);
//	}

	std::string filepath = request.server.getlocations()[0].getroot() + '/' + request.server.getlocations()[0].getindexes()[0];
		// okay this is legit fucked, but still better than hardcoding the filepath
	generateResponse(filepath, request);
	return _response;
	// todo: generate respons and return
}

char	**maptoenv(std::map<std::string, std::string> baseenv) {
	char **env = (char**) ft_calloc(baseenv.size() + 1, sizeof(char*));
	int i = 0;

	if (!env)
		exit(1);
	for (std::map<std::string, std::string>::const_iterator it = baseenv.begin(); it != baseenv.end(); it++) {
		std::string tmp = it->first + "=" + it->second;
		env[i] = ft_strdup(tmp.c_str());
		if (!env[i])
			exit(1);
		++i;
	}
	return env;
}

int	RequestHandler::run_cgi(request_s& request) {
	std::map<std::string, std::string> mappie = request.server.getbaseenv();
	pid_t	pid;
	char	**env = maptoenv(mappie);
	int pipefd[2];
	
	if (pipe(pipefd) == -1) {
		strerror(errno);
		exit(1);
	}
	if ((pid = fork()) == -1) {
		strerror(errno);
		exit(1);
	}
	else if (pid == 0) {
		dup2(pipefd[1], 1);
		close(pipefd[0]);
		int ret = execve("cgi-bin/printenv2.pl", NULL, env);
		if (ret == -1)
			std::cerr << "execve: " << strerror(errno) << std::endl;
		exit(1);
	}
	close(pipefd[1]);
	for (int i = 0; env[i]; i++) 
		free(env[i]);
	free(env);
	return pipefd[0];
}

void RequestHandler::generateResponse(const std::string& filepath, request_s& request) {
	int fd;
	this->_response = "HTTP/1.1 200 OK\n"
			   "Server: Webserv/0.1\n"
			   "Content-Type: text/html\n"
			   "Content-Length: 678\n\n";
	std::cout << "uri: " << request.uri << std::endl;
	if (CGI)
		fd = this->run_cgi(request);
	else
		fd = open(filepath.c_str(), O_RDONLY);
	if (fd == -1)
		throw std::runtime_error(strerror(errno));
	int ret;
	char buf[1024];
	do {
		ret = read(fd, buf, 1024);
		this->_response += buf;
		memset(buf, 0, 1024);
	} while (ret > 0);
	this->_response  += "\n";
	close(fd);
	// std::cout << "Response: \n" << _response << std::endl;
}

void RequestHandler::handleACCEPT_CHARSET(const std::string &value) {
	std::cout << "Value is: " << value << std::endl;
}

void RequestHandler::handleACCEPT_LANGUAGE(const std::string &value) {
	std::cout << "Value is: " << value << std::endl;
}

void RequestHandler::handleALLOW(const std::string &value) {
	std::cout << "Value is: " << value << std::endl;
}

void RequestHandler::handleAUTHORIZATION(const std::string &value) {
	std::cout << "Value is: " << value << std::endl;
}

void RequestHandler::handleCONTENT_LANGUAGE(const std::string &value) {
	std::cout << "Value is: " << value << std::endl;
}

void RequestHandler::handleCONTENT_LENGTH(const std::string &value) {
	std::cout << "Value is: " << value << std::endl;
}

void RequestHandler::handleCONTENT_LOCATION(const std::string &value) {
	std::cout << "Value is: " << value << std::endl;
}

void RequestHandler::handleCONTENT_TYPE(const std::string &value) {
	std::cout << "Value is: " << value << std::endl;
}

void RequestHandler::handleDATE(const std::string &value) {
	std::cout << "Value is: " << value << std::endl;
}

void RequestHandler::handleHOST(const std::string &value) {
	std::cout << "Value is: " << value << std::endl;
}

void RequestHandler::handleLAST_MODIFIED(const std::string &value) {
	std::cout << "Value is: " << value << std::endl;
}

void RequestHandler::handleLOCATION(const std::string &value) {
	std::cout << "Value is: " << value << std::endl;
}

void RequestHandler::handleREFERER(const std::string &value) {
	std::cout << "Value is: " << value << std::endl;
}

void RequestHandler::handleRETRY_AFTER(const std::string &value) {
	std::cout << "Value is: " << value << std::endl;
}

void RequestHandler::handleSERVER(const std::string &value) {
	std::cout << "Value is: " << value << std::endl;
}

void RequestHandler::handleTRANSFER_ENCODING(const std::string &value) {
	std::cout << "Value is: " << value << std::endl;
}

void RequestHandler::handleUSER_AGENT(const std::string &value) {
	std::cout << "Value is: " << value << std::endl;
}

void RequestHandler::handleWWW_AUTHENTICAT(const std::string &value) {
	std::cout << "Value is: " << value << std::endl;
}
