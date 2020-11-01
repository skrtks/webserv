/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   RequestHandler.cpp                                 :+:    :+:            */
/*                                                     +:+                    */
/*   By: skorteka <skorteka@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/10/08 16:15:11 by skorteka      #+#    #+#                 */
/*   Updated: 2020/10/17 16:25:51 by peerdb        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "RequestHandler.hpp"
#include "libftGnl.hpp"
#include <fcntl.h>
#include <zconf.h>
#include <sys/stat.h>
#include "Base64.hpp"
#include "Colours.hpp"

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

RequestHandler& RequestHandler::operator=(const RequestHandler &obj) {
	if (this != &obj) {
		this->_functionMap = obj._functionMap;
		this->_response = obj._response;
	}
	return *this;
}

std::string RequestHandler::handleRequest(request_s request) {
	std::cout << "Server for this request is: " << request.server.getservername() << std::endl; // todo: remove this for production
//	for (std::map<headerType, std::string>::iterator it=request.headers.begin(); it!=request.headers.end(); it++) {
//		(this->*(_functionMap.at(it->first)))(it->second);
//	}
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

int	RequestHandler::run_cgi(const request_s& request) {
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

int RequestHandler::authenticate(request_s& request) {
	std::string username, passwd, str;
	std::cerr << "beginning of authenticate.\n";
	try {
		std::cerr << "beginning of try block.\n";
		std::string auth = request.headers.at(AUTHORIZATION);
		std::string type, credentials;
		get_key_value(auth, type, credentials);
		credentials = base64_decode(credentials);
		get_key_value(credentials, username, passwd, ":");
	}
	catch (std::exception& e) {
		std::cerr << "turns out its not giving us login information." << std::endl;
	}
	std::cerr << "after catch block\n";
	int htpasswd_fd = open(request.server.gethtpasswdpath().c_str(), O_RDONLY);
	std::cerr << "fd = " << htpasswd_fd << std::endl;
	if (htpasswd_fd < 0 ) {
		std::cerr << "invalid file\n";
		return 1;
	}
	std::cerr << "after opening file\n";
	while (int ret = ft::get_next_line(htpasswd_fd, str) > 0) {
		std::cerr << "ret = " << ret << ", after reading line str: " << str << std::endl;
		std::string u, p;
		get_key_value(str, u, p, ":");
		std::cerr << "after getting key value\n";
		p = base64_decode(p);
		std::cerr << "decoded reference username/pass = " << u << "&" << p << std::endl;
		if (username == u && passwd == p) {
			close(htpasswd_fd);
			return 0;
		}
	}
	std::cerr << "bitchboy\n";
	this->_response =	"HTTP/1.1 401 Unauthorized\n"
				   		"Server: Webserv/0.1\n"
					  	"Content-Type: text/html\n"
	   					"WWW-Authenticate: Basic realm= ";
	this->_response += request.server.getauthbasicrealm();
	_response += "\n\n";
	std::cerr << "_response: " << _response;
	close(htpasswd_fd);
	return 1;
}

void RequestHandler::generateResponse(request_s& request) {
	int			fd = -1;
	struct stat	statstruct = {};
	std::string filepath = request.server.getroot() + request.uri;

	if (this->authenticate(request) )
		return ;
	this->_response =	"HTTP/1.1 200 OK\n"
				   		"Server: Webserv/0.1\n"
		 				"Content-Type: text/html\n"
	   					"Content-Length: 678\n\n";

	if (request.uri.compare(0, 9, "/cgi-bin/") == 0 && request.uri.length() > 9)	// Run CGI script that creates an html page
		fd = this->run_cgi(request);
	else if (stat(filepath.c_str(), &statstruct) != -1) {
		if (statstruct.st_size > request.server.getclientbodysize()) // should this account for images that are in the embedded in the html page? How would you check that?
			std::cerr << "Cant serve requested file, filesize is " << statstruct.st_size << ". Client body limit is " << request.server.getclientbodysize() << std::endl;
		else if (S_ISDIR(statstruct.st_mode)) {											// In case of a directory, we serve index.html
			filepath = request.server.getroot() + '/' + request.server.getindex();	// We don't do location matching just yet.
			fd = open(filepath.c_str(), O_RDONLY);
		}
		else
			fd = open(filepath.c_str(), O_RDONLY);									// Serving [rootfolder]/[URI]
	}
	if (fd == -1) {
		filepath = request.server.getroot() + '/' + request.server.geterrorpage();	// Serving the default error page
		fd = open(filepath.c_str(), O_RDONLY);
	}
	if (fd == -1)
		throw std::runtime_error(strerror(errno)); // cant even pull up the error page
	int ret;
	char buf[1024];
	do {
		ret = read(fd, buf, 1024);
		this->_response += buf;
		memset(buf, 0, 1024);
	} while (ret > 0);
	this->_response  += "\n";
	close(fd);
}

void RequestHandler::handleACCEPT_CHARSET(const std::string &value) {
	std::cout << "ACCEPT_CHARSET: " << value << std::endl;
}

void RequestHandler::handleACCEPT_LANGUAGE(const std::string &value) {
	std::cout << "ACCEPT_LANGUAGE: " << value << std::endl;
}

void RequestHandler::handleALLOW(const std::string &value) {
	std::cout << "ALLOW: " << value << std::endl;
}

void RequestHandler::handleAUTHORIZATION(const std::string &value) {
	std::cout << "AUTHORIZATION: " << value << std::endl;
}

void RequestHandler::handleCONTENT_LANGUAGE(const std::string &value) {
	std::cout << "CONTENT_LANGUAGE: " << value << std::endl;
}

void RequestHandler::handleCONTENT_LENGTH(const std::string &value) {
	std::cout << "CONTENT_LENGTH: " << value << std::endl;
}

void RequestHandler::handleCONTENT_LOCATION(const std::string &value) {
	std::cout << "CONTENT_LOCATION: " << value << std::endl;
}

void RequestHandler::handleCONTENT_TYPE(const std::string &value) {
	std::cout << "CONTENT_TYPE: " << value << std::endl;
}

void RequestHandler::handleDATE(const std::string &value) {
	std::cout << "DATE: " << value << std::endl;
}

void RequestHandler::handleHOST(const std::string &value) {
	std::cout << "HOST: " << value << std::endl;
}

void RequestHandler::handleLAST_MODIFIED(const std::string &value) {
	std::cout << "LAST_MODIFIED: " << value << std::endl;
}

void RequestHandler::handleLOCATION(const std::string &value) {
	std::cout << "LOCATION: " << value << std::endl;
}

void RequestHandler::handleREFERER(const std::string &value) {
	std::cout << "REFERER: " << value << std::endl;
}

void RequestHandler::handleRETRY_AFTER(const std::string &value) {
	std::cout << "RETRY_AFTER: " << value << std::endl;
}

void RequestHandler::handleSERVER(const std::string &value) {
	std::cout << "SERVER: " << value << std::endl;
}

void RequestHandler::handleTRANSFER_ENCODING(const std::string &value) {
	std::cout << "TRANSFER_ENCODING: " << value << std::endl;
}

void RequestHandler::handleUSER_AGENT(const std::string &value) {
	std::cout << "USER_AGENT: " << value << std::endl;
}

void RequestHandler::handleWWW_AUTHENTICAT(const std::string &value) {
	std::cout << "WWW_AUTHENTICAT: " << value << std::endl;
}
