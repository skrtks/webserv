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
#include <sys/stat.h>
#include "Base64.hpp"
#include "Colours.hpp"

ResponseHandler::ResponseHandler() {
	this->_body_length = -1;
	this->_status_code = 200;
	_header_vals[ACCEPT_CHARSET].clear();
	_header_vals[ACCEPT_LANGUAGE].clear();
	_header_vals[ALLOW].clear();
	_header_vals[AUTHORIZATION].clear();
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

	_status_codes[100] = "100 Continue\n"; // Only a part of the request has been received by the server, but as long as it has not been rejected, the client should continue with the request.
	_status_codes[200] = "200 OK\n";
	_status_codes[201] = "201 Created\n";
	_status_codes[202] = "202 Accepted\n"; // The request is accepted for processing, but the processing is not complete.
	_status_codes[203] = "203 Non-Authoritative Information\n"; // The information in the entity header is from a local or third-party copy, not from the original server.
	_status_codes[204] = "204 No Content\n";
	_status_codes[300] = "300 Multiple Choices\n"; // A link list. The user can select a link and go to that location. Maximum five addresses  .
	_status_codes[301] = "301 Moved Permanently\n";
	_status_codes[302] = "302 Found\n"; // The requested page has moved temporarily to a new url .
	_status_codes[307] = "307 Temporary Redirect\n"; // The requested page has moved temporarily to a new url.
	_status_codes[400] = "400 Bad Request\n"; // The server did not understand the request.
	_status_codes[404] = "404 Not Found\n";
	_status_codes[405] = "405 Method Not Allowed\n";
	_status_codes[406] = "406 Not Acceptable\n";
	_status_codes[407] = "407 Proxy Authentication Required\n";
	_status_codes[408] = "408 Request Timeout\n";
	_status_codes[409] = "409 Conflict\n"; // The request could not be completed because of a conflict.
	_status_codes[410] = "410 Gone\n"; // The requested page is no longer available .
	_status_codes[411] = "411 Length Required\n"; // The "Content-Length" is not defined. The server will not accept the request without it .
	_status_codes[413] = "413 Request Entity Too Large\n";
	_status_codes[414] = "414 Request-url Too Long\n";
	_status_codes[415] = "415 Unsupported Media Type\n";
	_status_codes[500] = "500 Internal Server Error\n"; // The request was not completed. The server met an unexpected condition.
	_status_codes[501] = "501 Not Implemented\n"; // The request was not completed. The server did not support the functionality required.
	_status_codes[502] = "502 Bad Gateway\n"; // The request was not completed. The server received an invalid response from the upstream server.
	_status_codes[503] = "503 Service Unavailable\n"; // The request was not completed. The server is temporarily overloading or down.
	_status_codes[504] = "504 Gateway Timeout\n";
	_status_codes[505] = "505 HTTP Version Not Supported\n";
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

char	**maptoenv(std::map<std::string, std::string> baseenv, const request_s& req) {
	int i = 0;
	char **env = (char**) ft_calloc(baseenv.size() + 1, sizeof(char*));
	if (!env)
		exit(1);
	(void)req;
	baseenv["AUTH_TYPE"] = "Basic"; //hardcoded for now
//	for (std::map<headerType, std::string>::const_iterator it = req.headers.begin(); it != req.headers.end(); ++it) {
//		std::cout << "request.headers contains: " << it->first << " -> " << it->second << std::endl;
//	}

	for (std::map<std::string, std::string>::const_iterator it = baseenv.begin(); it != baseenv.end(); it++) {
		std::string tmp = it->first + "=" + it->second;
		env[i] = ft_strdup(tmp.c_str());
		if (!env[i])
			exit(1);
		++i;
	}
	return env;
}

int	ResponseHandler::run_cgi(const request_s& request) {
	std::string		scriptpath = request.uri.substr(1, request.uri.length() - 1);
	pid_t			pid;
	struct stat		statstruct = {};
	char			*args[2] = {&scriptpath[0], NULL};

	if (stat(scriptpath.c_str(), &statstruct) == -1)
		return (-1);
	std::cout << _BLUE << "scriptpath: " << scriptpath << std::endl << _END;
	char	**env = maptoenv(request.server.getbaseenv(), request);
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
		int ret = execve(scriptpath.c_str(), args, env);
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

int ResponseHandler::generatePage(request_s& request) {
	int			fd = -1;

	if (request.uri.compare(0, 9, "/cgi-bin/") == 0 && request.uri.length() > 9) // Run CGI script that creates an html page
		fd = this->run_cgi(request);
	else
		fd = request.server.getpage(request.uri, _header_vals, _status_code);
	if (fd == -1)
		throw std::runtime_error(strerror(errno)); // cant even serve the error page, so I throw an error
	return (fd);
}

void ResponseHandler::handleBody(request_s& request) {
	int		ret = 0;
	char	buf[1024];
	int		fd = 0;

	_body_length = 0;
	_body.clear();
	if (request.status_code == 400)
		fd = open("./htmlfiles/error.html", O_RDONLY);
	else
		fd = generatePage(request);
	do {
		ret = read(fd, buf, 1024);
		if (ret <= 0)
			break ;
		_body_length += ret;
		_body.append(buf, ret);
		memset(buf, 0, 1024);
	} while (ret > 0);
	close(fd);
}

std::string ResponseHandler::handleRequest(request_s& request) {
	std::cout << "Server for above request is: " << request.server.getservername() << std::endl;
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
	std::vector<std::string> AllowedMethods = request.server.matchlocation(request.uri).getallowmethods();
	bool PutIsAllowed = false;
	for (std::vector<std::string>::const_iterator it = AllowedMethods.begin(); it != AllowedMethods.end(); ++it)
		if (*it == "PUT")
			PutIsAllowed = true;

	std::string filePath = request.server.getfilepath(request.uri);
	int statret = stat(filePath.c_str(), &statstruct);

	if (!PutIsAllowed) {
		_response += "405 Method Not Allowed\n";
	}
	else {
		int fd = open(filePath.c_str(), O_TRUNC | O_CREAT | O_WRONLY, S_IRWXU);
		if (fd != -1) {
			if (statret == -1)
				this->_response += "201 Created\n";
			else this->_response += "204 No Content\n";
			size_t WriteRet = write(fd, request.body.c_str(), request.body.length());
			close(fd);
			if (WriteRet != request.body.length())
				throw std::runtime_error(_RED _BOLD "Write return in ResponseHandler::handlePut is not equal to request.body.length()");
		}
		else {
			this->_response += "500 Internal Server Error\n";
			std::cerr << _RED "strerror: " << strerror(errno) << std::endl << _END;
		}
	}
	_response += "\r\n";
}

void ResponseHandler::generateResponse(request_s& request) {
	this->_status_code = 200;
	_response = "HTTP/1.1 ";
	 if (this->authenticate(request))
	 	return;
	if (request.status_code)
		this->_status_code = request.status_code;
	handleBody(request);
	handleStatusCode(request);
	handleALLOW();
	handleDATE();
	handleCONTENT_LANGUAGE();
	handleCONTENT_LENGTH();
	handleCONTENT_LOCATION();
	handleCONTENT_TYPE(request);
	handleSERVER();
	handleHOST(request);
	_response += "\n";
	_response += _body;
	_response += "\r\n";
//	std::cout << "RESPONSE == \n" << _response << std::endl;
}

int ResponseHandler::authenticate(request_s& request) {
	if (request.server.gethtpasswdpath().empty())
		return 0;
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

	if (request.server.getmatch(username, passwd)) {
		std::cout << _GREEN << "Authorization successful!" << _END << std::endl;
		return 0;
	}

	std::cout << _RED << "Authorization failed!" << _END << std::endl;
	this->_status_code = 401;
	_response += "401 Unauthorized\n";
	this->_response +=	"Server: Webserv/0.1\n"
					  	"Content-Type: text/html\n"
	   					"WWW-Authenticate: Basic realm=";
	this->_response += request.server.getauthbasicrealm();
	this->_response += ", charset=\"UTF-8\"\n\n";
	return 1;
}

void	ResponseHandler::handleStatusCode(request_s& request) {
	if (request.version.first != 1 && _status_code == 200)
		_status_code = 505;
	_response += _status_codes[_status_code];
}


std::string ResponseHandler::getCurrentDatetime(void ) {
	time_t		time;
	char 		datetime[100];
	std::string dtRet;
	tm*			curr_time;
	
	// gettimeofday(&time, NULL);
	std::time(&time);
	curr_time = std::localtime(&time);
	std::strftime(datetime, 100, "%a, %d %B %Y %T GMT", curr_time);
	dtRet = datetime;
	return (dtRet);
}

void ResponseHandler::handleALLOW( void ) {
	_header_vals[ALLOW] = "GET, HEAD";
	_response += "Allow: ";
	_response += _header_vals[ALLOW];
	_response += "\n";
	//std::cout << "ALLOW Value is: " << _header_vals[ALLOW] << std::endl;
}

void ResponseHandler::handleCONTENT_LANGUAGE( void ) {
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
	_response += "Content-Language: ";
	_response += _header_vals[CONTENT_LANGUAGE];
	_response += "\n";
}

void ResponseHandler::handleCONTENT_LENGTH( void ) {
	std::stringstream	ss;
	std::string			str;

	ss << _body_length;
	str = ss.str();
	_header_vals[CONTENT_LENGTH] = str;
	_response += "Content-Length: ";
	_response += _header_vals[CONTENT_LENGTH];
	_response += "\n";
}

void ResponseHandler::handleCONTENT_LOCATION( void ) {
	if (!_header_vals[CONTENT_LOCATION].empty()) {
		_response += "Content-Location: ";
		_response += _header_vals[CONTENT_LOCATION];
		_response += "\n";
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
	_response += "Content-Type: ";
	_response += _header_vals[CONTENT_TYPE];
	_response += "\n";
}

void ResponseHandler::handleDATE( void ) {
	_header_vals[DATE] = getCurrentDatetime();
	_response += "Date: ";
	_response += _header_vals[DATE];
	_response += "\n";
}

void ResponseHandler::handleHOST( request_s& request ) {
	std::stringstream ss;
	ss << request.server.gethost() << ":" << request.server.getport();
	_header_vals[HOST] = ss.str();
	//std::cout << "HOST: " << _header_vals[HOST] << std::endl;
}

void ResponseHandler::handleLAST_MODIFIED( void ) {
	std::cout << "Value is: " << "NULL" << std::endl;
}

void ResponseHandler::handleLOCATION( void ) {
	std::cout << "Value is: " << "NULL" << std::endl;
}

void ResponseHandler::handleRETRY_AFTER( void ) {
	std::cout << "Value is: " << "NULL" << std::endl;
}

void ResponseHandler::handleSERVER( void ) {
	_response += "Server: Webserv/1.0\n";
}

void ResponseHandler::handleTRANSFER_ENCODING( request_s& request ) {
	_header_vals[TRANSFER_ENCODING] = "identity";
	if (_body_length > request.server.getclientbodysize() || _body_length < 0) // arbitrary number, docs state that chunked transfer encoding is usually used for mb/gb onwards datatransfers
	{
		
	}
	//std::cout << "TRANSFER_ENCODING Value is: " << _header_vals[TRANSFER_ENCODING] << std::endl;
}
