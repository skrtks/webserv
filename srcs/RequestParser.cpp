/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   RequestParser.cpp                                  :+:    :+:            */
/*                                                     +:+                    */
/*   By: sam <sam@student.codam.nl>                   +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/10/06 12:01:35 by sam           #+#    #+#                 */
/*   Updated: 2020/11/29 14:00:32 by tuperera      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Colours.hpp"
#include "RequestParser.hpp"
#include "libftGnl.hpp"

RequestParser::RequestParser() {
	_status_code = 0;
	_methodMap["GET"] = GET;
	_methodMap["HEAD"] = HEAD;
	_methodMap["POST"] = POST;
	_methodMap["PUT"] = PUT;
	_headerMap["ACCEPT-CHARSET"] = ACCEPT_CHARSET;
	_headerMap["ACCEPT-LANGUAGE"] = ACCEPT_LANGUAGE;
	_headerMap["ALLOW"] = ALLOW;
	_headerMap["AUTHORIZATION"] = AUTHORIZATION;
	_headerMap["CONTENT-LANGUAGE"] = CONTENT_LANGUAGE;
	_headerMap["CONTENT-LENGTH"] = CONTENT_LENGTH;
	_headerMap["CONTENT-LOCATION"] = CONTENT_LOCATION;
	_headerMap["CONTENT-TYPE"] = CONTENT_TYPE;
	_headerMap["DATE"] = DATE;
	_headerMap["HOST"] = HOST;
	_headerMap["LAST-MODIFIED"] = LAST_MODIFIED;
	_headerMap["LOCATION"] = LOCATION;
	_headerMap["REFERER"] = REFERER;
	_headerMap["RETRY-AFTER"] = RETRY_AFTER;
	_headerMap["SERVER"] = SERVER;
	_headerMap["TRANSFER-ENCODING"] = TRANSFER_ENCODING;
	_headerMap["USER-AGENT"] = USER_AGENT;
	_headerMap["WWW-AUTHENTICATE"] = WWW_AUTHENTICATE;
}

RequestParser::~RequestParser() {
}

RequestParser::RequestParser(const RequestParser &obj) : _status_code() {
	*this = obj;
}

RequestParser& RequestParser::operator= (const RequestParser &obj) {
	if (this != &obj) {
		this->_method = obj._method;
		this->_uri = obj._uri;
		this->_version = obj._version;
		this->_headers = obj._headers;
		this->_methodMap = obj._methodMap;
		this->_headerMap = obj._headerMap;
		this->_rawRequest = obj._rawRequest;
	}
	return *this;
}

request_s RequestParser::parseRequest(const std::string &req) {
	request_s request;
	_rawRequest = req;
	_headers.clear();
	_status_code = 0;
	
	parseRequestLine();
	if (_status_code == 0)
		parseHeaders();
	if (_status_code == 0) {
		request.headers = _headers;
		request.method = _method;
		request.version = _version;
		request.uri = _uri;
	}
	request.body = _rawRequest; // Replace this with parseBody()
	std::map<headerType, std::string>::iterator it;

//	if (!_headers[CONTENT_LENGTH].empty()) {
//		int length = ft_atoi(_headers[CONTENT_LENGTH].c_str());
//		if (_headers[CONTENT_LENGTH].find_first_not_of("0123456789") != std::string::npos || length < 0)
//			_status_code = 400;
//	}
	if (_status_code)
		request.status_code = _status_code;
	else
		request.status_code = 0;
	return (request);
}

void RequestParser::parseRequestLine() {
	size_t eoRequestLine = _rawRequest.find("\r\n", 0);
	size_t pos = 0;
	size_t pos2 = 0;

	// Check if first char is space
	if (_rawRequest[0] == ' ') {
		std::cerr << "BAD REQ 1" << std::endl;
		_status_code = 400;
		return ;
	}
	if (_rawRequest.find("\r\n", 0) == std::string::npos) {
		std::cerr << "BAD REQ 2" << std::endl;
		_status_code = 400;
		return ;
	}
	// Check if there is 2 spaces in pline
	int numSpaces = 0;
	int doubleSpace = 0;
	for (int i = 0; _rawRequest[i] != '\r'; i++) {
		if (_rawRequest[i] == ' ')
			numSpaces++;
		if (_rawRequest[i] == ' ' && _rawRequest[i + 1] == ' ')
			doubleSpace = 1;
	}
	if (numSpaces != 2 || doubleSpace == 1) {
		std::cerr << "BAD REQ 3" << std::endl;
		_status_code = 400;
		return ;
	}
	extractMethod(eoRequestLine, pos);
	pos++;
	extractUri(eoRequestLine, pos, pos2);
	pos++;
	if (_uri.length() > 10000000) {
		std::cerr << "BAD REQ 3.1" << std::endl;
		_status_code = 414;
		return ;
	}
	extractVersion(eoRequestLine, pos, pos2);
	// Remove Request Line from _rawRequest
	_rawRequest.erase(0, pos + 5);
}

void RequestParser::extractVersion(size_t eoRequestLine, size_t &pos, size_t &pos2) {
	int mainVersion;
	int subVersion;
	std::string ret;

	pos = _rawRequest.find("HTTP/", pos);
	if (pos > eoRequestLine) {
		std::cerr << "BAD REQ 3.2" << std::endl;
		_status_code = 400;
		return ;
	}
	pos += 5;
	pos2 = _rawRequest.find("\r\n", pos);
	if (pos2 > eoRequestLine) {
		std::cerr << "BAD REQ 4" << std::endl;
		_status_code = 400;
		return ;
	}
	ret = _rawRequest.substr(pos, pos2 - pos);
	mainVersion = ft_atoi(ret.c_str());
	subVersion = ft_atoi(ret.c_str() + 2);
	_version = std::make_pair(mainVersion, subVersion);
}

void RequestParser::extractUri(size_t eoRequestLine, size_t pos, size_t pos2) {
	std::string ret;

	pos2 = _rawRequest.find(' ', pos);
	if (pos2 > eoRequestLine) {
		std::cerr << "BAD REQ 5" << std::endl;
		_status_code = 400;
		return ;
	}
	ret = _rawRequest.substr(pos, pos2 - pos);
	_uri = ret;
	if (_uri[0] == ':') {
		std::cerr << "BAD REQ5.1" << std::endl;
		_status_code = 400;
		return ;
	}
}

void RequestParser::extractMethod(size_t eoRequestLine, size_t& pos) {
	std::string ret;

	pos = _rawRequest.find(' ', 0);
	if (pos > eoRequestLine) {
		std::cerr << "BAD REQ 6" << std::endl;
		_status_code = 400;
		return ;
	}
	ret = _rawRequest.substr(0, pos);
	std::map<std::string, e_method>::iterator it = _methodMap.find(ret);
	if (it != _methodMap.end()) {
		_method = it->second;
	}
	else {
		std::cerr << "BAD REQ 7: cant find method (" << ret << ") in _methodMap" << std::endl;
		_status_code = 400;
		return ;
	}
}

// Parse headers and store them in _headers (map)
void RequestParser::parseHeaders() {
	std::string upperHeader;
	int owsOffset;
	while (!_rawRequest.empty()) {
		upperHeader.clear();
		size_t eoRequestLine = _rawRequest.find("\r\n", 0);
		if (eoRequestLine != 0 && std::string::npos != eoRequestLine) {
			if (_rawRequest[0] == ' ') {
				std::cerr << "BAD REQ 8" << std::endl;
				_status_code = 400;
				return ;
			}
			size_t pos = _rawRequest.find(':', 0);
			if (pos > eoRequestLine) {
				std::cerr << "BAD REQ 9" << std::endl;
				_status_code = 400;
				return ;
			}
			std::string header = _rawRequest.substr(0, pos);
			if (header.empty() || header.length() == 0) {
				std::cerr << "BAD REQ 10.1" << std::endl;
				_status_code = 400;
				return ;
			}
			for (size_t i = 0; header[i]; i++) {
				if (header[i] == ' ') {
					std::cerr << "BAD REQ 10" << std::endl;
					_status_code = 400;
					return ;
				}
			}
			pos++;
			// 'Skip' over OWS at beginning of value string
			for (int i = pos; _rawRequest[i] == ' '; i++)
				pos++;
			owsOffset = 0;
			// Create offset for OWS at end of value string
			for (size_t i = eoRequestLine - 1; i != std::string::npos && _rawRequest[i] == ' '; --i) {
				owsOffset++;
			}
			// Extract value string and check if not empty or beginning with newline
			std::string value = _rawRequest.substr(pos, eoRequestLine - pos - owsOffset);
			if (value.empty() || _rawRequest[pos] == '\r') {
				std::cerr << _rawRequest << std::endl;
				std::cerr << "BAD REQ 11" << std::endl;
				_status_code = 400;
				return ;
			}
			for (int i = 0; header[i]; i++) {
				upperHeader += std::toupper(header[i]);
			}
			// Match found header to correct headerType using map
			std::map<std::string, headerType>::iterator it = _headerMap.find(upperHeader);
			if (it != _headerMap.end()) {
				if (_headers.find(it->second) != _headers.end()) {
					std::cerr << "BAD REQ 12" << std::endl;
					_status_code = 400;
					return ;
				}
				_headers.insert(std::make_pair(it->second, value));
			}
		}
		else {
			eoRequestLine = _rawRequest.find("\r\n", 0);
			if (eoRequestLine == std::string::npos) {
				std::cerr << "2. BAD REQ 7: cant find '\\r\\n' in _rawRequest" << std::endl;
				_status_code = 400;
				return ;
			}
			_rawRequest.erase(0, eoRequestLine + 2);
			return;
		}
		// Erase processed line
		_rawRequest.erase(0, eoRequestLine + 2);
	}
}

// MARK: getters & setters

const std::map<headerType, std::string>& RequestParser::getHeaders() const {
	return _headers;
}

e_method RequestParser::getMethod() const {
	return _method;
}

const std::string& RequestParser::getUri() const {
	return _uri;
}

void RequestParser::setRawRequest(const std::string& rawRequest) {
	_rawRequest = rawRequest;
}

const std::pair<int, int>& RequestParser::getVersion() const {
	return _version;
}

std::string RequestParser::getMethod(e_method &x) {
	switch (x) {
		case GET:
			return "GET";
		case HEAD:
			return "HEAD";
		case POST:
			return "POST";
		case PUT:
			return "PUT";
		default:
			return "NOMETHOD";
	}
}
