/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   ResponseHandler.cpp                                 :+:    :+:            */
/*                                                     +:+                    */
/*   By: skorteka <skorteka@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/10/08 16:15:11 by skorteka      #+#    #+#                 */
/*   Updated: 2020/10/29 11:08:24 by tuperera      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "ResponseHandler.hpp"
#include "libftGnl.hpp"
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include "Base64.hpp"
#include "Colours.hpp"

std::string getCurrentDatetime() {
	time_t		time;
	char 		datetime[100];
	tm*			curr_time;

	std::time(&time);
	curr_time = std::localtime(&time);
	std::strftime(datetime, 100, "%a, %d %B %Y %H:%M:%S GMT", curr_time);
	return datetime;
}

ResponseHandler::ResponseHandler() : _cgi_status_code() {
	this->_status_code = 200;
	_header_vals[ACCEPT_CHARSET].clear();
	_header_vals[ACCEPT_LANGUAGE].clear();
	_header_vals[ALLOW].clear();
	_header_vals[AUTHORIZATION].clear();
	_header_vals[CONNECTION] = "keep-alive";
	_header_vals[CONTENT_LANGUAGE].clear();
	_header_vals[CONTENT_LENGTH].clear();
	_header_vals[CONTENT_LOCATION].clear();
	_header_vals[CONTENT_TYPE].clear();
	_header_vals[DATE].clear();
	_header_vals[HOST].clear();
	_header_vals[LAST_MODIFIED].clear();
	_header_vals[LOCATION].clear();
	_header_vals[RETRY_AFTER].clear();
	_header_vals[SERVER] = "Webserv/1.0";
	_header_vals[TRANSFER_ENCODING].clear();
	_header_vals[WWW_AUTHENTICATE].clear();

	_status_codes[100] = "100 Continue\r\n"; // Only a part of the request has been received by the server, but as long as it has not been rejected, the client should continue with the request.
	_status_codes[200] = "200 OK\r\n";
	_status_codes[201] = "201 Created\r\n";
	_status_codes[202] = "202 Accepted\r\n"; // The request is accepted for processing, but the processing is not complete.
	_status_codes[203] = "203 Non-Authoritative Information\r\n"; // The information in the entity header is from a local or third-party copy, not from the original server.
	_status_codes[204] = "204 No Content\r\n";
	_status_codes[300] = "300 Multiple Choices\r\n"; // A link list. The user can select a link and go to that location. Maximum five addresses  .
	_status_codes[301] = "301 Moved Permanently\r\n";
	_status_codes[302] = "302 Found\r\n"; // The requested page has moved temporarily to a new url .
	_status_codes[307] = "307 Temporary Redirect\r\n"; // The requested page has moved temporarily to a new url.
	_status_codes[400] = "400 Bad Request\r\n"; // The server did not understand the request.
	_status_codes[404] = "404 Not Found\r\n";
	_status_codes[405] = "405 Method Not Allowed\r\n";
	_status_codes[406] = "406 Not Acceptable\r\n";
	_status_codes[407] = "407 Proxy Authentication Required\r\n";
	_status_codes[408] = "408 Request Timeout\r\n";
	_status_codes[409] = "409 Conflict\r\n"; // The request could not be completed because of a conflict.
	_status_codes[410] = "410 Gone\r\n"; // The requested page is no longer available .
	_status_codes[411] = "411 Length Required\r\n"; // The "Content-Length" is not defined. The server will not accept the request without it .
	_status_codes[413] = "413 Request Entity Too Large\r\n";
	_status_codes[414] = "414 Request-url Too Long\r\n";
	_status_codes[415] = "415 Unsupported Media Type\r\n";
	_status_codes[500] = "500 Internal Server Error\r\n"; // The request was not completed. The server met an unexpected condition.
	_status_codes[501] = "501 Not Implemented\r\n"; // The request was not completed. The server did not support the functionality required.
	_status_codes[502] = "502 Bad Gateway\r\n"; // The request was not completed. The server received an invalid response from the upstream server.
	_status_codes[503] = "503 Service Unavailable\r\n"; // The request was not completed. The server is temporarily overloading or down.
	_status_codes[504] = "504 Gateway Timeout\r\n";
	_status_codes[505] = "505 HTTP Version Not Supported\r\n";
}

ResponseHandler::~ResponseHandler() {
	this->_header_vals.clear();
	this->_cgi_headers.clear();
	this->_status_codes.clear();
	this->_response.clear();
	this->_body.clear();
}

ResponseHandler::ResponseHandler(const ResponseHandler &src) : _cgi_status_code(), _status_code() {
	*this = src;
}

ResponseHandler& ResponseHandler::operator= (const ResponseHandler &rhs) {
	if (this != &rhs) {
		this->_header_vals = rhs._header_vals;
		this->_cgi_headers = rhs._cgi_headers;
		this->_cgi_status_code = rhs._cgi_status_code;
		this->_status_codes = rhs._status_codes;
		this->_response	= rhs._response;
		this->_body = rhs._body;
		this->_status_code = rhs._status_code;
		this->CGI = rhs.CGI;
	}
	return *this;
}

int ResponseHandler::generatePage(request_s& request) {
	int			fd = -1;
	struct stat statstruct = {};

	if (request.server->isExtensionAllowed(request.uri)) {
		std::string scriptpath = request.uri.substr(1, request.uri.find_first_of('/', request.uri.find_first_of('.') ) - 1);
		if (request.uri.compare(0, 9, "/cgi-bin/") == 0 && request.uri.length() > 9) { // Run CGI script that creates an html page
			fd = this->CGI.run_cgi(request, scriptpath, request.uri);
		}
		else {
			std::string tmpuri = '/' + request.server->matchlocation(request.uri).getroot();
			size_t second_slash_index = request.uri.find_first_of('/', 1);
			if (second_slash_index == std::string::npos)
				tmpuri += request.uri;
			else
				tmpuri += request.uri.substr(second_slash_index);
			scriptpath = tmpuri.substr(1, tmpuri.find_first_of('/', tmpuri.find_first_of('.') ) - 1);
			if (stat(scriptpath.c_str(), &statstruct) == -1) {
				std::string defaultcgipath = request.server->matchlocation(request.uri).getdefaultcgipath();
				if (defaultcgipath.empty()) {
					fd = -2;
				}
				else {
					second_slash_index = tmpuri.find_first_of('/', 1);
					tmpuri.replace(second_slash_index + 1, tmpuri.find_first_of('/', second_slash_index), defaultcgipath);
				}
			}
			if (fd != -2) {
				std::string	OriginalUri(request.uri);
				request.uri = tmpuri;
				scriptpath = request.uri.substr(1, request.uri.find_first_of('/', request.uri.find_first_of('.')) - 1);
				fd = this->CGI.run_cgi(request, scriptpath, OriginalUri);
			}
		}
	}
	else
		fd = request.server->getpage(request.uri, _header_vals, _status_code);
	if (fd == -1)
		throw std::runtime_error(strerror(errno)); // cant even serve the error page, so I throw an error
	if (fd == -2) {
		fd = open(request.server->geterrorpage().c_str(), O_RDONLY);
		_status_code = 404;
	}
	return (fd);
}

void ResponseHandler::extractCGIheaders(const std::string& incoming) {
	RequestParser TMP;
	std::vector<std::string> vec = ft::split(incoming, "\n");
	for (std::vector<std::string>::iterator it = vec.begin(); it != vec.end(); ++it) {
		if ((*it).find(':') == std::string::npos)
			continue;
		std::string key, value;
		get_key_value(*it, key, value, ":");
		ft::stringtoupper(key);
		ft::trimstring(value);
		std::map<std::string, headerType>::iterator header = TMP._headerMap.find(key);
		if (header != TMP._headerMap.end()) {
			_cgi_headers[header->second] = value;
		}
		else if (key == "STATUS")
			_cgi_status_code = ft_atoi(value.c_str());
	}
}

void ResponseHandler::handleBody(request_s& request) {
	int		ret = 1024;
	char	buf[1024];
	int		fd;
	int totalreadsize = 0;

	_body.clear();
	if (request.status_code == 400) {
		fd = open(request.server->matchlocation(request.uri).geterrorpage().c_str(), O_RDONLY);
	}
	else {
		fd = generatePage(request);
	}
	while (ret == 1024) {
		ret = read(fd, buf, 1024);
		if (ret <= 0)
			break;
		totalreadsize += ret;
		_body.append(buf, ret);
		memset(buf, 0, 1024);
	}
	if (close(fd) == -1) {
		exit(EXIT_FAILURE);
	}
	size_t pos = _body.find("\r\n\r\n");
	if (request.method == POST) {
		this->extractCGIheaders(_body.substr(0, pos + 4));
		if (pos != std::string::npos)
			_body.erase(0, pos + 4);
	}

}
std::string& ResponseHandler::handleRequest(request_s& request) {
	this->_response.clear();
	if (request.method == PUT) {
		handlePut(request);
	}
	else {
		generateResponse(request);
	}
	return _response;
}

void ResponseHandler::handlePut(request_s& request) {
	struct stat statstruct = {};
	_response = "HTTP/1.1 ";

	std::string filePath = request.server->getfilepath(request.uri);
	int statret = stat(filePath.c_str(), &statstruct);

	if (!request.server->matchlocation(request.uri).checkifMethodAllowed(request.method)) {
		_status_code = 405;
		_body.clear();
	}
	else {
		int fd = open(filePath.c_str(), O_TRUNC | O_CREAT | O_WRONLY, S_IRWXU);
		if (fd != -1) {
			this->_response += _status_codes[204];
			size_t WriteRet = write(fd, request.body.c_str(), request.body.length());
			close(fd);
			if (WriteRet != request.body.length())
				throw std::runtime_error(_RED _BOLD "Write return in ResponseHandler::handlePut is not equal to request.body.length()");
			handleLOCATION(filePath);
		}
		else {
			this->_response += _status_codes[500];
		}
	}
//	handleCONNECTION_HEADER(request);
	_response += "\r\n";
}

void ResponseHandler::generateResponse(request_s& request) {
	this->_status_code = 200;
	_response = "HTTP/1.1 ";

	if (!request.server->matchlocation(request.uri).checkifMethodAllowed(request.method)) {
		_status_code = 405;
		_body.clear();
	}
	if (this->authenticate(request))
		return;
	if (request.body.length() > request.server->matchlocation(request.uri).getmaxbody()) { // If body length is higher than location::maxBody
		request.status_code = 413;
	}
	if (request.status_code)
		this->_status_code = request.status_code;

	handleBody(request);
	handleStatusCode(request);
	handleCONTENT_TYPE(request);
	handleALLOW();
	handleDATE();
	handleCONTENT_LENGTH();
	handleCONTENT_LOCATION();
	handleCONTENT_LANGUAGE();
	handleSERVER();
//	handleCONNECTION_HEADER(request);
	_response += "\r\n";
	if (request.method != HEAD && !_body.empty()) {
		_response += _body;
	}
	_body.clear();
}

int ResponseHandler::authenticate(request_s& request) {
	if (request.server->gethtpasswdpath().empty()) {
		request.headers[AUTHORIZATION].clear();
		return 0;
	}
	std::string username, passwd, str;
	try {
		std::string auth = request.headers.at(AUTHORIZATION);
		std::string type, credentials;
		get_key_value(auth, type, credentials);
		credentials = base64_decode(credentials);
		get_key_value(credentials, username, passwd, ":");
	}
	catch (std::exception& e) {
		std::cerr << "No credentials provided by client" << std::endl;
	}
	request.headers[AUTHORIZATION] = request.headers[AUTHORIZATION].substr(0, request.headers[AUTHORIZATION].find_first_of(' '));
	request.headers[REMOTE_USER] = username;
	if (request.server->getmatch(username, passwd)) {
		std::cout << _GREEN "Authorization successful!" _END << std::endl;
		return 0;
	}

	std::cout << _RED "Authorization failed!" _END << std::endl;
	this->_status_code = 401;
	_response += "401 Unauthorized\r\n";
	this->_response +=	"Server: Webserv/0.1\r\n"
					  	"Content-Type: text/html\r\n"
	   					"WWW-Authenticate: Basic realm=";
	this->_response += request.server->getauthbasicrealm();
	this->_response += ", charset=\"UTF-8\"\r\n";
	return 1;
}

void	ResponseHandler::handleStatusCode(request_s& request) {
	if (this->_status_code == 200 && _cgi_status_code)
		_status_code = _cgi_status_code;
	if (request.version.first != 1 && _status_code == 200)
		_status_code = 505;
	_response += _status_codes[_status_code];
}

void ResponseHandler::handleALLOW() {
	_header_vals[ALLOW] = "GET, HEAD, POST, PUT";
	_response += "Allow: ";
	_response += _header_vals[ALLOW];
	_response += "\r\n";
}

void ResponseHandler::handleCONTENT_LANGUAGE() {
	std::string lang;
	size_t	found = _body.find("<html");
	size_t	lang_idx = _body.find("lang", found + 1);

	if (lang_idx != std::string::npos)
	{
		for (size_t i = lang_idx + 6; _body[i] != '\"'; i++)
			lang += _body[i];
		_header_vals[CONTENT_LANGUAGE] = lang;
	}
	else
	{
		_header_vals[CONTENT_LANGUAGE] = "en-US";
	}
	_response += "Content-Language: ";
	_response += _header_vals[CONTENT_LANGUAGE];
	_response += "\r\n";
}

void ResponseHandler::handleCONTENT_LENGTH() {
	std::stringstream	ss;
	std::string			str;

	if (!_body.empty())
		_header_vals[CONTENT_LENGTH] = ft::inttostring(_body.length());
	else
		_header_vals[CONTENT_LENGTH] = "0";
	_response += "Content-Length: ";
	_response += _header_vals[CONTENT_LENGTH];
	_response += "\r\n";
}

void ResponseHandler::handleCONTENT_LOCATION() {
	if (!_header_vals[CONTENT_LOCATION].empty()) {
		_response += "Content-Location: ";
		_response += _header_vals[CONTENT_LOCATION];
		_response += "\r\n";
	}
}

void ResponseHandler::handleCONTENT_TYPE(request_s& request) {
	// Defaults to html if no css is found
	if (_cgi_headers.count(CONTENT_TYPE) == 1) {
		_header_vals[CONTENT_TYPE] = _cgi_headers[CONTENT_TYPE];
	}
	if (request.uri.find(".css") != std::string::npos) {
		_header_vals[CONTENT_TYPE] = "text/css";
	}
	else if (request.uri.find(".ico") != std::string::npos) {
		_header_vals[CONTENT_TYPE] = "image/x-icon";
	}
	else if (request.uri.find(".jpg") != std::string::npos || request.uri.find(".jpeg") != std::string::npos) {
		_header_vals[CONTENT_TYPE] = "image/jpeg";
	}
	else {
		_header_vals[CONTENT_TYPE] = "text/html";
	}
	request.headers[CONTENT_TYPE] = this->_header_vals[CONTENT_TYPE];
	_response += "Content-Type: ";
	_response += _header_vals[CONTENT_TYPE];
	_response += "\r\n";
}

void ResponseHandler::handleDATE() {
	_header_vals[DATE] = getCurrentDatetime();
	_response += "Date: ";
	_response += _header_vals[DATE];
	_response += "\r\n";
}

void ResponseHandler::handleLOCATION( std::string& url ) {
	_header_vals[LOCATION] = url;
	_response += "Location: ";
	_response += url;
	_response += "\r\n";
}

void ResponseHandler::handleSERVER() {
	_response += "Host: Webserv/1.0\r\n";
}

void ResponseHandler::handleCONNECTION_HEADER(const request_s& request) {
	if (request.headers.count(CONNECTION) == 1)
		_header_vals[CONNECTION] = request.headers.at(CONNECTION);
	_response += "Connection: " + _header_vals[CONNECTION] + "\r\n";
}

