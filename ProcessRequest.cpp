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
	_headerMap["ACCEPT_CHARSET"] = ACCEPT_CHARSET;
	_headerMap["ACCEPT_LANGUAGE"] = ACCEPT_LANGUAGE;
	_headerMap["ALLOW"] = ALLOW;
	_headerMap["AUTHORIZATION"] = AUTHORIZATION;
	_headerMap["CONTENT_LANGUAGE"] = CONTENT_LANGUAGE;
	_headerMap["CONTENT_LENGTH"] = CONTENT_LENGTH;
	_headerMap["CONTENT_LOCATION"] = CONTENT_LOCATION;
	_headerMap["CONTENT_TYPE"] = CONTENT_TYPE;
	_headerMap["DATE"] = DATE;
	_headerMap["HOST"] = HOST;
	_headerMap["LAST_MODIFIED"] = LAST_MODIFIED;
	_headerMap["LOCATION"] = LOCATION;
	_headerMap["REFERER"] = REFERER;
	_headerMap["RETRY_AFTER"] = RETRY_AFTER;
	_headerMap["SERVER"] = SERVER;
	_headerMap["TRANSFER_ENCODING"] = TRANSFER_ENCODING;
	_headerMap["USER_AGENT"] = USER_AGENT;
	_headerMap["WWW_AUTHENTICATE"] = WWW_AUTHENTICATE;
}

ProcessRequest::~ProcessRequest() {
}

void ProcessRequest::parseRequest(const std::string &req) {
	_rawRequest = req;
	parseRequestLine();
	parseHeaders();
	// TODO: extract headers
	// TODO: execute header
	// TODO: generate response
	// TODO: return respons
}

void ProcessRequest::parseRequestLine() {
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

// Parse headers and store them in _headers (map)
void ProcessRequest::parseHeaders() {
	std::string upperHeader;
	int owsOffset;
	while (!_rawRequest.empty()) {
		upperHeader.clear();
		size_t eoRequestLine = _rawRequest.find("\r\n", 0);
		if (eoRequestLine != 0) {
			if (_rawRequest[0] == ' ')
				throw std::runtime_error("Invalid syntax"); // TODO: replace with correct http error
			size_t pos = _rawRequest.find(':', 0);
			if (pos > eoRequestLine)
				throw std::runtime_error("Invalid syntax"); // TODO: replace with correct http error
			std::string header = _rawRequest.substr(0, pos);
			for (char i : header) {
				if (i == ' ')
					throw std::runtime_error("Invalid syntax"); // TODO: replace with correct http error
			}
			pos++;
			// 'Skip' over OWS at beginning of value string
			for (int i = pos; _rawRequest[i] == ' '; i++)
				pos++;
			owsOffset = 0;
			// Create offset for OWS at end of value string
			for (int i = eoRequestLine - 1; i >= 0 && _rawRequest[i] == ' '; --i) {
				owsOffset++;
			}
			// Extract value string and check if not empty or beginning with newline
			std::string value = _rawRequest.substr(pos, eoRequestLine - pos - owsOffset);
			if (value.empty() || _rawRequest[pos] == '\r') {
				throw std::runtime_error("Empty value"); // TODO: replace with correct http error
			}
			for (int i = 0; header[i]; i++) {
				upperHeader += std::toupper(header[i]);
			}
			// Match found header to correct headerType using map
			std::__1::map<std::string, headerType>::iterator it = _headerMap.find(upperHeader);
			if (it != _headerMap.end()) {
				_headers.insert(std::make_pair(it->second, value));
			}
		}
		// Erase processed line
		_rawRequest.erase(0, eoRequestLine + 2);
	}
}

const std::map<headerType, std::string>& ProcessRequest::getHeaders() const {
	return _headers;
}
