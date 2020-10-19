/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   RequestHandler.cpp                                 :+:    :+:            */
/*                                                     +:+                    */
/*   By: skorteka <skorteka@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/10/08 16:15:11 by skorteka      #+#    #+#                 */
/*   Updated: 2020/10/18 17:16:00 by tuperera      ########   odam.nl         */
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
	_functionMap[WWW_AUTHENTICATE] = &RequestHandler::handleWWW_AUTHENTICATE;
}

RequestHandler::~RequestHandler() {
}

RequestHandler::RequestHandler(const RequestHandler &src) {
	this->_functionMap	= src._functionMap;
	this->_header_vals	= src._header_vals;
	this->_response		= src._response;
}

RequestHandler& RequestHandler::operator= (const RequestHandler &rhs) {
	if (this != &rhs) {
		this->_functionMap	= rhs._functionMap;
		this->_header_vals		= rhs._header_vals;
		this->_response		= rhs._response;
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
	generateResponse(filepath);
	return _response;
	// todo: generate respons and return
}

void RequestHandler::generateResponse(const std::string& filepath) {
	int 	fd = 0;
	int		ret = 0;
	char	buf[1024];
	if (fd == -1)
		throw std::runtime_error(strerror(errno));

	fd = open(filepath.c_str(), O_RDONLY);
	_body_length = 0;
	_body = "";
	do {
		ret = read(fd, buf, 1024);
		if (ret <= 0)
			break ;
		_body_length += ret;
		_body.append(buf, ret);
		memset(buf, 0, 1024);
	} while (ret > 0);
	std::cout << "BODY = " << _body << std::endl;
	handleACCEPT_CHARSET();
	handleACCEPT_LANGUAGE();
	handleALLOW();
	handleCONTENT_LANGUAGE();
	handleCONTENT_TYPE();
	handleDATE();
	handleCONTENT_LENGTH();
	_response = "HTTP/1.1 200 OK\n"
			   "Server: Webserv/0.1\n"
			   "Content-Type: text/html\n"
			   "Content-Length: 678\n\n";
	_response += _body;
	_response += "\r\n";
	close(fd);
	// std::cout << "Response: \n" << _response << std::endl;
}

void RequestHandler::handleACCEPT_CHARSET( void ) {
	_header_vals[ACCEPT_CHARSET] = "utf-8; q=0.9"; // this should take ACCEPT_CHARSET value of request unless we dont support that charset if so error
	//std::cout << "ACCEPT_CHARSET Value is: " << _header_vals[ACCEPT_CHARSET] << std::endl;
}

void RequestHandler::handleACCEPT_LANGUAGE( void ) {
	_header_vals[ACCEPT_LANGUAGE] = "en-US,en;q=0.9";
	//std::cout << "ACCEPT_LANGUAGE Value is: " << _header_vals[ACCEPT_LANGUAGE] << std::endl;
}

void RequestHandler::handleALLOW( void ) {
	_header_vals[ALLOW] = "GET, HEAD"; // TODO find out how to do this dynamically
	//std::cout << "ALLOW Value is: " << _header_vals[ALLOW] << std::endl;
}

void RequestHandler::handleAUTHORIZATION( void ) {
	std::cout << "Value is: " << "NULL" << std::endl;
}

void RequestHandler::handleCONTENT_LANGUAGE( void ) {
	int		idx = 0;
	char	*lang = new char[100];
	size_t	found = 0;
	size_t	lang_idx = 0;
	
	found = _body.find("<html");
	lang_idx = _body.find("lang", found + 1);
	if (lang_idx)
	{
		for (size_t i = lang_idx + 6; _body[i] != '\"'; i++)
			lang[idx++] = _body[i];
		_header_vals[CONTENT_LANGUAGE] = lang;
	}
	else
	{
		_header_vals[CONTENT_LANGUAGE] = "en-US";
	}
	//std::cout << "CONTENT_LANGUAGE Value is: " << _header_vals[CONTENT_LANGUAGE] << std::endl;
	delete []lang;
}

void RequestHandler::handleCONTENT_LENGTH( void ) {
	std::stringstream	ss;
	std::string			str;

	ss << _body_length;
	str = ss.str();
	_header_vals[CONTENT_LENGTH] = str;

	//std::cout << "CONTENT_LENGTH Value is: " << _header_vals[CONTENT_LENGTH] << std::endl;
}

void RequestHandler::handleCONTENT_LOCATION( void ) {
	std::cout << "Value is: " << "NULL" << std::endl;
}

void RequestHandler::handleCONTENT_TYPE( void ) {
	// for now hardcoded too text/html but will need to be non-static if we serve other datatypes
	_header_vals[CONTENT_TYPE] = "text/html";

	//std::cout << "CONTENT_TYPE Value is: " << _header_vals[CONTENT_TYPE] << std::endl;
}

void RequestHandler::handleDATE( void ) {
	//struct timeval	time;
	time_t			time;
	char			datetime[100];
	tm*				curr_time;
	
	//gettimeofday(&time, NULL);
	std::time(&time);
	curr_time = std::localtime(&time);
	std::strftime(datetime, 100, "%a, %d %B %Y %T GMT", curr_time);
	_header_vals[DATE] = datetime;

	//std::cout << "DATE Value is: " << _header_vals[DATE] << std::endl;
}

void RequestHandler::handleHOST( void ) {
	std::cout << "Value is: " << "NULL" << std::endl;
}

void RequestHandler::handleLAST_MODIFIED( void ) {
	std::cout << "Value is: " << "NULL" << std::endl;
}

void RequestHandler::handleLOCATION( void ) {
	std::cout << "Value is: " << "NULL" << std::endl;
}

void RequestHandler::handleREFERER( void ) {
	std::cout << "Value is: " << "NULL" << std::endl;
}

void RequestHandler::handleRETRY_AFTER( void ) {
	std::cout << "Value is: " << "NULL" << std::endl;
}

void RequestHandler::handleSERVER( void ) {
	_header_vals[SERVER] = "Webserv/1.0";
	//std::cout << "SERVER Value is: " << _header_vals[SERVER] << std::endl;
}

void RequestHandler::handleTRANSFER_ENCODING( void ) {
	_header_vals[TRANSFER_ENCODING] = "identity";
	//std::cout << "TRANSFER_ENCODING Value is: " << _header_vals[TRANSFER_ENCODING] << std::endl;
}

void RequestHandler::handleUSER_AGENT( void ) {
	std::cout << "Value is: " << "NULL" << std::endl;
}

void RequestHandler::handleWWW_AUTHENTICATE( void ) {
	std::cout << "Value is: " << "NULL" << std::endl;
}
