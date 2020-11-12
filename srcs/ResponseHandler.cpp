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

ResponseHandler::ResponseHandler(const ResponseHandler &src) {
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

int	ResponseHandler::run_cgi(const request_s& request) {
	std::string		scriptpath = request.uri.substr(1, request.uri.length() - 1);
	pid_t			pid;
	struct stat		statstruct = {};
	char			*args[2] = {&scriptpath[0], NULL};

	if (stat(scriptpath.c_str(), &statstruct) == -1)
		return (-1);
	char	**env = maptoenv(request.server.getbaseenv());
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
	struct stat	statstruct = {};
	std::string filepath = request.server.getroot() + request.uri;

	if (request.uri.compare(0, 9, "/cgi-bin/") == 0 && request.uri.length() > 9)	// Run CGI script that creates an html page
		fd = this->run_cgi(request);
	else if (stat(filepath.c_str(), &statstruct) != -1) {
		if (statstruct.st_size > request.server.getclientbodysize()) // should this account for images that are in the embedded in the html page? How would you check that?
			std::cerr << _RED << "Cant serve requested file, filesize is " << statstruct.st_size << ". Client body limit is " << request.server.getclientbodysize() << std::endl << _END;
		else if (S_ISDIR(statstruct.st_mode)) {											// In case of a directory, we serve index.html
			std::cerr << _BLUE << filepath << " is a directory" << std::endl << _END;
			filepath = request.server.getroot() + '/' + request.server.getindex();	// We don't do location matching just yet.
			_header_vals[CONTENT_LOCATION] = filepath;
			fd = open(filepath.c_str(), O_RDONLY);
		}
		else {
			fd = open(filepath.c_str(), O_RDONLY);									// Serving [rootfolder]/[URI]
			std::cerr << _GREEN << filepath << " is a valid file, lets serve it at fd " << fd << std::endl << std::endl << _END;
		}
	}
	if (fd == -1) {
		std::cerr << _RED << "Serving the 404 error page" << std::endl << _END;
		_status_code = 404;
		filepath = request.server.getroot() + '/' + request.server.get404page();	// Serving the default error page
		_header_vals[CONTENT_LOCATION] = filepath;
		fd = open(filepath.c_str(), O_RDONLY);
	}
	if (fd == -1)
		throw std::runtime_error(strerror(errno)); // cant even serve the error page, so I throw an error
	return (fd);
}

void ResponseHandler::handleBody(request_s& request) {
	int		ret = 0;
	char	buf[1024];
	int		fd = 0;

	_body_length = 0;
	_body = "";
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

std::string ResponseHandler::handleRequest(request_s request) {
	std::cout << "Server for this request is: " << request.server.getservername() << std::endl; // todo: remove this for production
	generateResponse(request);
	return _response;
}

void ResponseHandler::generateResponse(request_s& request) {
	this->_status_code = 200;
	_response = "HTTP/1.1 ";
	std::cout << "STATUS CODE = " << request.status_code << std::endl;
	// if (this->authenticate(request))
	// 	std::cout << "Auth" << std::endl;
	if (request.status_code)
		this->_status_code = request.status_code;
	handleBody(request);
	handleStatusCode(request);
	handleALLOW();
	handleDATE();
	handleCONTENT_LANGUAGE();
	handleCONTENT_LENGTH();
	handleCONTENT_LOCATION();
	handleCONTENT_TYPE();
	handleSERVER();
	handleHOST(request);
	_response += "\n";
	_response += _body;
	_response += "\r\n";
	std::cout << "RESPONSE == \n" << _response << std::endl;
}

int ResponseHandler::authenticate(request_s& request) {
	bool creds = false;
	std::string username, passwd, str;
	try {
		std::string auth = request.headers.at(AUTHORIZATION);
		std::cerr << _YELLOW << "auth gives: " << auth	 << std::endl << _END;
		std::string type, credentials;
		get_key_value(auth, type, credentials);
		credentials = base64_decode(credentials);
		get_key_value(credentials, username, passwd, ":");
		std::cerr << "creds = true, username = " << username << ", pass = " << passwd << std::endl;
		creds = true;
		// request.headers.at(AUTHORIZATION) = "Basic -";
	}
	catch (std::exception& e) {
		std::cerr << "turns out its not giving us login information." << std::endl;
	}
	int htpasswd_fd = open(request.server.gethtpasswdpath().c_str(), O_RDONLY);
	if (htpasswd_fd < 0 ) {
		std::cerr << "htpasswd_path is invalid\n";
		return 1;
	}
	while (creds && ft::get_next_line(htpasswd_fd, str) > 0) {
		std::string u, p;
		get_key_value(str, u, p, ":");
		p = base64_decode(p);
		std::cerr << _GREEN "user tries to log in with: " << username << ":" << passwd << std::endl << _END;
		std::cerr << _CYAN "file: " << u << ":" << p << std::endl << _END;
		if (username == u && passwd == p) {
			close(htpasswd_fd);
			return 0;
		}
	}
	creds ? _response += "403 Forbidden\n" : _response += "401 Unauthorized\n";
	this->_response +=	"Server: Webserv/0.1\n"
					  	"Content-Type: text/html\n"
	   					"WWW-Authenticate: Basic realm=";
	this->_response += request.server.getauthbasicrealm();
	this->_response += ", charset=\"UTF-8\"\n\n";
	// std::cerr << "response: " << _response << std::endl;
	close(htpasswd_fd);
	return 1;
}

void	ResponseHandler::handleStatusCode(request_s& request) {
	if (request.version.first != 1 && _status_code == 200)
		_status_code = 505;
	_response += _status_codes[_status_code];
}


char*	ResponseHandler::getCurrentDatetime( void ) {
	time_t	time;
	char*	datetime = new char[100];
	tm*		curr_time;
	
	// gettimeofday(&time, NULL);
	std::time(&time);
	curr_time = std::localtime(&time);
	std::strftime(datetime, 100, "%a, %d %B %Y %T GMT", curr_time);
	return (datetime);
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
	if (_header_vals[CONTENT_LOCATION].empty() != true) {
		_response += "Content-Location: ";
		_response += _header_vals[CONTENT_LOCATION];
		_response += "\n";
	}
}

void ResponseHandler::handleCONTENT_TYPE( void ) {
	// for now hardcoded to text/html but will need to be non-static if we serve other datatypes
	_header_vals[CONTENT_TYPE] = "text/html";
	_response += "Content-Type: ";
	_response += _header_vals[CONTENT_TYPE];
	_response += "\n";
}

void ResponseHandler::handleDATE( void ) {
	// TODO: free the datetime values when done
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
