/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   ProcessRequest.cpp                                 :+:    :+:            */
/*                                                     +:+                    */
/*   By: sam <sam@student.codam.nl>                   +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/10/06 12:01:35 by sam           #+#    #+#                 */
/*   Updated: 2020/10/06 12:01:35 by sam           ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <cstdlib>
#include "ProcessRequest.hpp"

ProcessRequest::ProcessRequest() {
	_methodMap["GET"] = GET;
	_methodMap["HEAD"] = HEAD;
	_methodMap["POST"] = POST;
	_methodMap["PUT"] = PUT;
}

ProcessRequest::~ProcessRequest() {
}

void ProcessRequest::parseRequest(const std::string &req) {
	_rawRequest = req;
	processRequestLine();
	// TODO: extract headers
	// TODO: execute header
	// TODO: generate response
	// TODO: return respons
}

void ProcessRequest::processRequestLine() {
	size_t eoRequestLine = _rawRequest.find("\r\n", 0);
	size_t pos = 0;
	size_t pos2 = 0;

	// Check if first char is space
	if (_rawRequest[0] == ' ') {
		throw std::runtime_error("Forbidden WS"); // TODO: replace with correct http error
	}
	if (_rawRequest.find("\r\n", 0) == std::string::npos) {
		throw std::runtime_error("No CRLF found in request"); // TODO: replace with correct http error
	}
	// Check if there is 2 spaces in pline
	int numSpaces = 0;
	for (int i = 0; _rawRequest[i] != '\r'; i++) {
		if (_rawRequest[i] == ' ')
			numSpaces++;
	}
	if (numSpaces != 2) {
		throw std::runtime_error("Forbidden WS"); // TODO: replace with correct http error
	}
	extractMethod(eoRequestLine, pos);
	pos++;
	extractUri(eoRequestLine, pos, pos2);
	pos++;
	extractVersion(eoRequestLine, pos, pos2);
	// Remove Request Line from _rawRequest
	_rawRequest.erase(0, pos + 5);
}

void ProcessRequest::extractVersion(size_t eoRequestLine, size_t &pos, size_t &pos2) {
	int mainVersion;
	int subVersion;
	std::string ret;

	pos = _rawRequest.find("HTTP/", pos);
	if (pos > eoRequestLine)
		throw std::runtime_error("Error parsing version"); // TODO: replace with correct http error
	pos += 5;
	pos2 = _rawRequest.find("\r\n", pos);
	if (pos2 > eoRequestLine)
		throw std::runtime_error("Error parsing version"); // TODO: replace with correct http error
	ret = _rawRequest.substr(pos, pos2 - pos);
	mainVersion = std::atoi(ret.c_str()); // TODO: use ft_atoi
	subVersion = std::atoi(ret.c_str() + 2); // TODO: use ft_atoi
	_version = std::make_pair(mainVersion, subVersion);
}

void ProcessRequest::extractUri(size_t eoRequestLine, size_t pos, size_t pos2) {
	std::string ret;

	pos2 = _rawRequest.find(' ', pos);
	if (pos2 > eoRequestLine)
		throw std::runtime_error("Error parsing version"); // TODO: replace with correct http error
	ret = _rawRequest.substr(pos, pos2 - pos);
	_uri = ret;
}

void ProcessRequest::extractMethod(size_t eoRequestLine, size_t& pos) {
	std::string ret;

	pos = _rawRequest.find(' ', 0);
	if (pos > eoRequestLine)
		throw std::runtime_error("Error parsing version"); // TODO: replace with correct http error
	ret = _rawRequest.substr(0, pos);
	std::__1::map<std::string, method>::iterator it = _methodMap.find(ret);
	if (it != _methodMap.end()) {
		_method = it->second;
	}
	else throw std::runtime_error("Incorrect value for method"); // TODO: replace with correct http error
}

method ProcessRequest::getMethod() const {
	return _method;
}

const std::string& ProcessRequest::getUri() const {
	return _uri;
}

void ProcessRequest::setRawRequest(const std::string& rawRequest) {
	_rawRequest = rawRequest;
}

const std::pair<int, int>& ProcessRequest::getVersion() const {
	return _version;
}
