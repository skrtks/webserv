//
// Created by peerdb on 02-12-20.
//

#include "Cgi.hpp"

void	exit_fatal() {
	std::cerr << _RED _BOLD << strerror(errno) << std::endl << _END;
	exit(EXIT_FAILURE);
}

Cgi::Cgi() : _env() {

}

Cgi::Cgi(const Cgi &x) : _env() {
	*this = x;
}

Cgi &Cgi::operator=(const Cgi &x) {
	if (this != &x) {
		this->_m = x._m;
		this->_env = x._env;
	}
	return *this;
}

Cgi::~Cgi() {
	this->_m.clear();
}

void Cgi::populate_map(request_s &req) {
	int split_path = req.uri.find_first_of('/', req.uri.find_first_of('.') );
	char buf[500];
	std::string	realpath = getcwd(buf, 500);

	this->_m["AUTH_TYPE"] = req.headers[AUTHORIZATION];
	this->_m["CONTENT_LENGTH"] = req.headers[CONTENT_LENGTH];
	this->_m["CONTENT_TYPE"] = req.headers[CONTENT_TYPE]; //TODO fill this one
	this->_m["GATEWAY_INTERFACE"] = "CGI/1.1";
	this->_m["PATH_INFO"] = req.uri;
	this->_m["PATH_TRANSLATED"] = realpath + '/' + this->_m["PATH_INFO"];
	this->_m["QUERY_STRING"] = req.uri.substr(req.uri.find_first_of('?') + 1);
	this->_m["REMOTE_ADDR"] = req.server.gethost();
	this->_m["REMOTE_IDENT"] = ""; //TODO fill this one
	this->_m["REMOTE_USER"] = req.headers[REMOTE_USER];
	this->_m["REQUEST_METHOD"] = req.MethodToSTring();
	this->_m["REQUEST_URI"] = req.uri;
	this->_m["SCRIPT_NAME"] = req.uri.substr(0, split_path - 1 );
	this->_m["SERVER_NAME"] = req.server.getservername();
	this->_m["SERVER_PORT"] = std::string(ft::inttostring(req.server.getport()));
	this->_m["SERVER_PROTOCOL"] = "HTTP/1.1";
	this->_m["SERVER_SOFTWARE"] = "HTTP 1.1";
}

void	Cgi::map_to_env() {
	int i = 0;
	this->_env = (char**) ft_calloc(this->_m.size() + 1, sizeof(char*));
	if (!_env)
		exit_fatal();

	for (std::map<std::string, std::string>::const_iterator it = _m.begin(); it != _m.end(); it++) {
		std::string tmp = it->first + "=" + it->second;
		this->_env[i] = ft_strdup(tmp.c_str());
		if (!this->_env[i])
			exit_fatal();
		++i;
	}
}

void	Cgi::clear_env() {
	for (int i = 0; _env[i]; i++) {
		free(_env[i]);
		_env[i] = NULL;
	}
	free(_env);
	_env = NULL;
}

int Cgi::run_cgi(request_s &request) {
	pid_t			pid;
	struct stat		statstruct = {};
	int				outgoing_pipe[2],
					incoming_pipe[2];
	int 			split_path = request.uri.find_first_of('/', request.uri.find_first_of('.') );
	std::string		scriptpath = request.uri.substr(1, split_path - 1);
	char*			args[2] = {&scriptpath[0], NULL};


	if (stat(scriptpath.c_str(), &statstruct) == -1)
		return (open(request.server.geterrorpage().c_str(), O_RDONLY));

	this->populate_map(request);
	this->map_to_env();

	if (pipe(outgoing_pipe) == -1 || fcntl(outgoing_pipe[0], F_SETFL, O_NONBLOCK))
		exit_fatal();
//	if (request.method == POST)
	if (pipe(incoming_pipe) == -1 || fcntl(incoming_pipe[1], F_SETFL, O_NONBLOCK) == -1)
		exit_fatal();

	if ((pid = fork()) == -1)
		exit_fatal();
	if (pid == 0) {
		if (close(outgoing_pipe[0]) == -1 || dup2(outgoing_pipe[1], 1) == -1 || (close(outgoing_pipe[1]) == -1))
			exit_fatal();
//		if (request.method == POST) {
		if (close(incoming_pipe[1]) == -1 || dup2(incoming_pipe[0], 0) == -1 || close(incoming_pipe[0]) == -1)
			exit_fatal();
//		}
		if (execve(scriptpath.c_str(), args, _env) == -1)
			std::cerr << "execve: " << strerror(errno) << std::endl;
		exit(EXIT_FAILURE);
	}

	if (close(outgoing_pipe[1]) == -1)
		exit_fatal();
//	if (request.method == POST) {
	if (close(incoming_pipe[0]) == -1)
		exit_fatal();
	ssize_t dummy = write(incoming_pipe[1], request.body.c_str(), request.body.length()); // Child can read from the other end of this pipe
	(void)dummy;
	if (close(incoming_pipe[1]) == -1)
		exit_fatal();
//	}
	this->clear_env();
	return (outgoing_pipe[0]);
}
