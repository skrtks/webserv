/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   ExecuteHeaders.cpp                                 :+:    :+:            */
/*                                                     +:+                    */
/*   By: skorteka <skorteka@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/10/08 16:15:11 by skorteka      #+#    #+#                 */
/*   Updated: 2020/10/08 16:15:11 by skorteka      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "RequestHandler.hpp"
#include <iostream>
#include <fcntl.h>
#include <zconf.h>

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
	*this = obj;
}

RequestHandler& RequestHandler::operator== (const RequestHandler &obj) {
	if (this == &obj) {
		return *this;
	}
	*this = obj;
	return *this;
}

std::string RequestHandler::handleRequest(request_s request) {
	// todo: set method, set path to requested file, check http version
	for (std::map<headerType, std::string>::iterator it=request.headers.begin(); it!=request.headers.end(); it++) {
		(this->*(_functionMap.at(it->first)))(it->second);
	}
	generateResponse();
	return _response;
	// todo: generate respons and return
}

void RequestHandler::generateResponse() {
	_response = "HTTP/1.1 200 OK\n"
			   "Server: Webserv/0.1\n"
			   "Content-Type: text/html\n"
			   "Content-Length: 5336\n";
	int fd = open("/Users/skorteka/Desktop/vrijdag.html", O_RDONLY);
	if (fd == -1)
		throw std::runtime_error(strerror(errno));
	int ret;
	char buf[1024];
	do {
		ret = read(fd, buf, 1024);
		_response += buf;
		memset(buf, 0, 1024);
	} while (ret > 0);
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
