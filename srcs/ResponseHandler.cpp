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
#include "Colours.hpp"

ResponseHandler::ResponseHandler() : _body_length(-1) {
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

std::string ResponseHandler::handleRequest(request_s request) {
	std::cout << "Server for this request is: " << request.server.getservername() << std::endl; // todo: remove this for production
	generateResponse(request);
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
			std::cerr << _RED << _BOLD << "Cant serve requested file, filesize is " << statstruct.st_size << ". Client body limit is " << request.server.getclientbodysize() << std::endl << _END;
		else if (S_ISDIR(statstruct.st_mode)) {											// In case of a directory, we serve index.html
			std::cerr << _BLUE << _BOLD << filepath << " is a directory" << std::endl << _END;
			filepath = request.server.getroot() + '/' + request.server.getindex();	// We don't do location matching just yet.
			fd = open(filepath.c_str(), O_RDONLY);
		}
		else {
			fd = open(filepath.c_str(), O_RDONLY);									// Serving [rootfolder]/[URI]
			std::cerr << _GREEN << _BOLD << filepath << " is a valid file, lets serve it at fd " << fd << std::endl << std::endl << _END;
		}
	}
	if (fd == -1) {
		std::cerr << _RED << _BOLD << "Serving the default error page" << std::endl << _END;
		filepath = request.server.getroot() + '/' + request.server.geterrorpage();	// Serving the default error page
		fd = open(filepath.c_str(), O_RDONLY);
	}
	if (fd == -1)
		throw std::runtime_error(strerror(errno)); // cant even serve the error page, so I throw an error
	return (fd);
}

void ResponseHandler::generateResponse(request_s& request) {
	int		ret = 0;
	char	buf[1024];
	int		fd = 0;

	_body_length = 0;
	_body = "";

	fd = generatePage(request);
	do {
		ret = read(fd, buf, 1024);
		if (ret <= 0)
			break ;
		_body_length += ret;
		_body.append(buf, ret);
		memset(buf, 0, 1024);
	} while (ret > 0);
	handleACCEPT_CHARSET();
	handleACCEPT_LANGUAGE();
	handleALLOW();
	handleCONTENT_LANGUAGE();
	handleCONTENT_TYPE();
	handleDATE();
	handleCONTENT_LENGTH();
	handleUSER_AGENT(request);
	handleHOST(request);
	handleCONTENT_LOCATION(request);
	_response = "HTTP/1.1 200 OK\n"
			   "Server: Webserv/0.1\n"
			   "Content-Type: text/html\n"
			   "Content-Length: 678\n\n";
	_response += _body;
	_response += "\r\n";
	close(fd);
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

void ResponseHandler::handleACCEPT_CHARSET( void ) {
	_header_vals[ACCEPT_CHARSET] = "utf-8; q=0.9";
	//std::cout << "ACCEPT_CHARSET Value is: " << _header_vals[ACCEPT_CHARSET] << std::endl;
}

void ResponseHandler::handleACCEPT_LANGUAGE( void ) {
	_header_vals[ACCEPT_LANGUAGE] = "en-US,en;q=0.9";
	//std::cout << "ACCEPT_LANGUAGE Value is: " << _header_vals[ACCEPT_LANGUAGE] << std::endl;
}

void ResponseHandler::handleALLOW( void ) {
	_header_vals[ALLOW] = "GET, HEAD";
	//std::cout << "ALLOW Value is: " << _header_vals[ALLOW] << std::endl;
}

void ResponseHandler::handleAUTHORIZATION( void ) {
	_header_vals[AUTHORIZATION] = "";
	std::cout << "Value is: " << "NULL" << std::endl;
}

void ResponseHandler::handleCONTENT_LANGUAGE( void ) {
	int		idx = 0;
	char	*lang = new char[100];
	size_t	found = 0;
	size_t	lang_idx = 0;
	
	found = _body.find("<html");
	lang_idx = _body.find("lang", found + 1);
	if (lang_idx != std::string::npos)
	{
		for (size_t i = lang_idx + 6; _body[i] != '\"'; i++)
			lang[idx++] = _body[i];
		_header_vals[CONTENT_LANGUAGE] = lang;
	}
	else
	{
		_header_vals[CONTENT_LANGUAGE] = "en-US";
	}
	delete []lang;
}

void ResponseHandler::handleCONTENT_LENGTH( void ) {
	std::stringstream	ss;
	std::string			str;

	ss << _body_length;
	str = ss.str();
	_header_vals[CONTENT_LENGTH] = str;
}

void ResponseHandler::handleCONTENT_LOCATION(request_s& request) {
	// TODO
	std::cout << request.uri << std::endl;
}

void ResponseHandler::handleCONTENT_TYPE( void ) {
	// for now hardcoded too text/html but will need to be non-static if we serve other datatypes
	_header_vals[CONTENT_TYPE] = "text/html";

	//std::cout << "CONTENT_TYPE Value is: " << _header_vals[CONTENT_TYPE] << std::endl;
}

void ResponseHandler::handleDATE( void ) {
	// TODO: free the datetime values when done
	_header_vals[DATE] = getCurrentDatetime();
}

void ResponseHandler::handleHOST( request_s& request ) {
	std::stringstream ss;
	ss << request.server.gethost() << ":" << request.server.getport();
	_header_vals[HOST] = ss.str();
	std::cout << "HOST: " << _header_vals[HOST] << std::endl;
}

void ResponseHandler::handleLAST_MODIFIED( void ) {
	std::cout << "Value is: " << "NULL" << std::endl;
}

void ResponseHandler::handleLOCATION( void ) {
	std::cout << "Value is: " << "NULL" << std::endl;
}

// dont need this for server side responses
// void ResponseHandler::handleREFERER( void ) {
// 	std::cout << "Value is: " << "NULL" << std::endl;
// }

void ResponseHandler::handleRETRY_AFTER( void ) {
	std::cout << "Value is: " << "NULL" << std::endl;
}

void ResponseHandler::handleSERVER( void ) {
	_header_vals[SERVER] = "Webserv/1.0";
}

void ResponseHandler::handleTRANSFER_ENCODING( request_s& request ) {
	_header_vals[TRANSFER_ENCODING] = "identity";
	if (_body_length > request.server.getclientbodysize() || _body_length < 0) // arbitrary number, docs state that chunked transfer encoding is usually used for mb/gb onwards datatransfers
	{
		
	}
	//std::cout << "TRANSFER_ENCODING Value is: " << _header_vals[TRANSFER_ENCODING] << std::endl;
}

void ResponseHandler::handleUSER_AGENT( request_s& request ) {
	_header_vals[USER_AGENT] = request.headers[USER_AGENT];
	std::cout << "USER_AGENT: " << _header_vals[USER_AGENT] << std::endl;
}

void ResponseHandler::handleWWW_AUTHENTICATE( void ) {
	_header_vals[WWW_AUTHENTICATE] = "";
	std::cout << "Value is: " << "NULL" << std::endl;
}
