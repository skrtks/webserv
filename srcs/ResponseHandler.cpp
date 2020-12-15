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
#include <zconf.h>
#include <unistd.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include "Base64.hpp"
#include "Colours.hpp"

ResponseHandler::ResponseHandler() {
	this->_body_length = 0;
	this->_status_code = 200;
	_header_vals[ACCEPT_CHARSET].clear(); // TODO ??
	_header_vals[ACCEPT_LANGUAGE].clear();
	_header_vals[ALLOW].clear();
	_header_vals[AUTHORIZATION].clear();
	_header_vals[CONNECTION] = "close";
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
}

ResponseHandler::ResponseHandler(const ResponseHandler &src) : _body_length(), _status_code() {
	this->_header_vals = src._header_vals;
	this->_response	= src._response;
}

ResponseHandler& ResponseHandler::operator= (const ResponseHandler &rhs) {
	if (this != &rhs) {
		this->_header_vals = rhs._header_vals;
		this->_response	= rhs._response;
	}
	return *this;
}

int ResponseHandler::generatePage(request_s& request) {
	int			fd = -1;


	if (request.server.isExtensionAllowed(request.uri)) {
		if (request.uri.compare(0, 9, "/cgi-bin/") == 0 && request.uri.length() > 9) // Run CGI script that creates an html page
			fd = this->CGI.run_cgi(request);
		else {
			std::cerr << _RED _BOLD "uri not cgi-bin but valid cgi extension: " << request.uri << std::endl << _END;
			size_t second_slash_index = request.uri.find_first_of('/', 1);
			if (second_slash_index == std::string::npos)
				request.uri = '/' + request.server.matchlocation(request.uri).getroot() + request.uri;
			else
				request.uri.replace(1, second_slash_index - 1, request.server.matchlocation(request.uri).getroot());
			std::cerr << _CYAN _BOLD << "new uri is " << request.uri << ", second_slash_index used to be " << second_slash_index << _END << std::endl;
			fd = this->CGI.run_cgi(request);
		}
	}
	else
		fd = request.server.getpage(request.uri, _header_vals, _status_code);
	if (fd == -1)
		throw std::runtime_error(strerror(errno)); // cant even serve the error page, so I throw an error
	return (fd);
}

void ResponseHandler::handleBody(request_s& request) {
	int		ret = 1024;
	char	buf[1024];
	int		fd = 0;

	_body_length = 0;
	_body.clear();
	if (request.status_code == 400) {
		fd = open(request.server.matchlocation(request.uri).geterrorpage().c_str(), O_RDONLY);
	}
	else {
		fd = generatePage(request);
	}
	while (ret == 1024) {
		ret = read(fd, buf, 1024);
		if (ret <= 0)
			break ;
		_body_length += ret;
		_body.append(buf, ret);
		memset(buf, 0, 1024);
	}
	if (close(fd) == -1) {
		exit(EXIT_FAILURE);
	}
}

std::vector<std::string> ResponseHandler::handleRequest(request_s& request) {
	std::cout << "Server for above request is: " << request.server.getservername() << std::endl;
	this->_body_length = request.body.length();
	this->_response.resize(1);
	_response.front() = "";
//	std::cerr << _RED << "in handleRequest, _response immediately has size " << _response.size() << std::endl << _END;
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
	_response[0] = "HTTP/1.1 ";

	std::string filePath = request.server.getfilepath(request.uri);
	int statret = stat(filePath.c_str(), &statstruct);

	if (!request.server.matchlocation(request.uri).checkifMethodAllowed(request.method)) {
		_status_code = 405;
		_body.clear();
		_body_length = _body.length();
	}
	else {
		int fd = open(filePath.c_str(), O_TRUNC | O_CREAT | O_WRONLY, S_IRWXU);
		if (fd != -1) {
			if (statret == -1)
				this->_response[0] += "201 Created\r\n";
			else this->_response[0] += "204 No Content\r\n";
			size_t WriteRet = write(fd, request.body.c_str(), request.body.length());
			close(fd);
			if (WriteRet != request.body.length())
				throw std::runtime_error(_RED _BOLD "Write return in ResponseHandler::handlePut is not equal to request.body.length()");
		}
		else {
			this->_response[0] += "500 Internal Server Error\r\n";
			std::cerr << _RED "strerror: " << strerror(errno) << std::endl << _END;
		}
	}
	_response[0] += "\r\n";
	handleLOCATION(filePath);
	_response[0] += "\r\n";
	_response[0] += "\r\n";
}

void ResponseHandler::generateResponse(request_s& request) {
	this->_status_code = 200;
	_response[0] = "HTTP/1.1 ";

	std::cerr << request;

	if (!request.server.matchlocation(request.uri).checkifMethodAllowed(request.method)) {
		_status_code = 405;
		_body.clear();
		_body_length = _body.length();
	}
	 if (this->authenticate(request))
	  	return;
	 if (this->_body_length > request.server.matchlocation(request.uri).getmaxbody()) { // If body length is higher than location::maxBody
	 	this->_status_code = 413;
	 	return;
	 }
	if (request.status_code)
		this->_status_code = request.status_code;

	handleBody(request);
	handleStatusCode(request);
	handleCONTENT_TYPE(request); //TODO Do we need to do this before handleBody( ) ?
	handleALLOW();
	handleDATE();
	handleCONTENT_LENGTH();
	handleCONTENT_LOCATION();
	handleCONTENT_LANGUAGE(); //TODO Do we need to do this before handleBody( )
	handleSERVER();
	handleCONNECTION_HEADER();
	_response[0] += "\r\n";
	if (request.method != HEAD) {
		_response[0] += _body;
		_response[0] += "\r\n";
	}
	_body.clear();
}

int ResponseHandler::authenticate(request_s& request) {
	if (request.server.gethtpasswdpath().empty()) {
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
	if (request.server.getmatch(username, passwd)) {
		std::cout << _GREEN "Authorization successful!" _END << std::endl;
		return 0;
	}

	std::cout << _RED "Authorization failed!" _END << std::endl;
	this->_status_code = 401;
	_response[0] += "401 Unauthorized\r\n";
	this->_response[0] +=	"Server: Webserv/0.1\r\n"
					  	"Content-Type: text/html\r\n"
	   					"WWW-Authenticate: Basic realm=";
	this->_response[0] += request.server.getauthbasicrealm();
	this->_response[0] += ", charset=\"UTF-8\"\r\n";
	return 1;
}

void	ResponseHandler::handleStatusCode(request_s& request) {
	if (request.version.first != 1 && _status_code == 200)
		_status_code = 505;
	_response[0] += _status_codes[_status_code];
	std::cerr << _RED "Status code: " << _response[0] << std::endl << _END;
}


std::string ResponseHandler::getCurrentDatetime() {
	time_t		time;
	char 		datetime[100];
	std::string dtRet;
	tm*			curr_time;
	
	// gettimeofday(&time, NULL);
	std::time(&time);
	curr_time = std::localtime(&time);
	std::strftime(datetime, 100, "%a, %d %B %Y %H:%M:%S GMT", curr_time);
	dtRet = datetime;
	return (dtRet);
}

void ResponseHandler::handleALLOW() {
	_header_vals[ALLOW] = "GET, HEAD, POST, PUT";
	_response[0] += "Allow: ";
	_response[0] += _header_vals[ALLOW];
	_response[0] += "\r\n";
	//std::cout << "ALLOW Value is: " << _header_vals[ALLOW] << std::endl;
}

void ResponseHandler::handleCONTENT_LANGUAGE() {
//	int		idx = 0;
	std::string lang;
	size_t	found = 0;
	size_t	lang_idx = 0;
	
	found = _body.find("<html");
	lang_idx = _body.find("lang", found + 1);
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
	_response[0] += "Content-Language: ";
	_response[0] += _header_vals[CONTENT_LANGUAGE];
	_response[0] += "\r\n";
}

void ResponseHandler::handleCONTENT_LENGTH() {
	std::stringstream	ss;
	std::string			str;

	_header_vals[CONTENT_LENGTH] = ft::inttostring(_body_length);
	_response[0] += "Content-Length: ";
	_response[0] += _header_vals[CONTENT_LENGTH];
	_response[0] += "\r\n";
}

void ResponseHandler::handleCONTENT_LOCATION() {
	if (!_header_vals[CONTENT_LOCATION].empty()) {
		_response[0] += "Content-Location: ";
		_response[0] += _header_vals[CONTENT_LOCATION];
		_response[0] += "\r\n";
	}
}

void ResponseHandler::handleCONTENT_TYPE(request_s& request) {
	// Defaults to html if no css is found
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
	_response[0] += "Content-Type: ";
	_response[0] += _header_vals[CONTENT_TYPE];
	_response[0] += "\r\n";
}

void ResponseHandler::handleDATE() {
	_header_vals[DATE] = getCurrentDatetime();
	_response[0] += "Date: ";
	_response[0] += _header_vals[DATE];
	_response[0] += "\r\n";
}

void ResponseHandler::handleHOST( request_s& request ) {
	std::stringstream ss;
	ss << request.server.gethost() << ":" << request.server.getport();
	_header_vals[HOST] = ss.str();
	//std::cout << "HOST: " << _header_vals[HOST] << std::endl;
}

void ResponseHandler::handleLAST_MODIFIED() {
	_response[0] += "Last-Modified: ";
	_response[0] += getCurrentDatetime();
	_header_vals[LAST_MODIFIED] = getCurrentDatetime();
	_response[0] += "\r\n";
}

void ResponseHandler::handleLOCATION( std::string& url ) {
	_header_vals[LOCATION] = url;
	_response[0] += "Location: ";
	_response[0] += url;
	_response[0] += "\r\n";
	std::cout << "Location is: " << url << std::endl;
}

void ResponseHandler::handleRETRY_AFTER() {
	_response[0] += "Retry-After: 120\r\n";
}

void ResponseHandler::handleSERVER() {
	_response[0] += "Server: Webserv/1.0\r\n";
}

void ResponseHandler::handleCONNECTION_HEADER() {
	_response[0] += "Connection: " + _header_vals[CONNECTION] + "\r\n";
	_response[0] += "Accept-Encoding: gzip\r\n";
}

void ResponseHandler::handleTRANSFER_ENCODING( request_s& request ) {
	std::stringstream ss;
	std::string response;
	int i = 0;
	_header_vals[TRANSFER_ENCODING] = "chunked";
	response += "Transfer-Encoding: chunked\r\n\r\n";
	request.transfer_buffer = true;
	while (_body.length() > 10000) {
		ss << std::hex << 10000;
		response += ss.str();
		ss.str("");
		response += "\r\n";
		response.append(_body, 0, 10000);
		response += "\r\n";
		_body.erase(0, 10000);
		if (i == 0)
			_response[0] += response;
		else
			_response.push_back(response);
		response.clear();
		i++;
	}
	ss << std::hex << _body.length();
	response += ss.str();
	ss.str("");
	response += "\r\n";
	response.append(_body, 0, _body.length());
	_body.clear();
	response += "\r\n";
	if (i == 0)
		_response[0] += response;
	else
		_response.push_back(response);
	response.clear();
	i++;
	response += "0\r\n\r\n";
	_response.push_back(response);
}
