//
// Created by peerdb on 02-12-20.
//

#include "Cgi.hpp"
#include <sys/wait.h>

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

//	std::cerr << _GREEN << "body.size() is " << req.body.size() << std::endl;
	this->_m["AUTH_TYPE"] = req.headers[AUTHORIZATION];
//	this->_m["CONTENT_LENGTH"] = req.headers[CONTENT_LENGTH];
	this->_m["CONTENT_LENGTH"] = ft::inttostring(req.body.size());
	this->_m["CONTENT_TYPE"] = req.headers[CONTENT_TYPE]; //TODO fill this one
	this->_m["GATEWAY_INTERFACE"] = "CGI/1.1";
	this->_m["PATH_INFO"] = req.uri;
	this->_m["PATH_TRANSLATED"] = realpath + this->_m["PATH_INFO"];
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

void Cgi::map_to_env(request_s& request) {
	int i = 0;
	this->_m.insert(request.env.begin(), request.env.end());
	std::cerr << _YELLOW << "Inserted the following headers:\n";
	for (std::map<std::string, std::string>::const_iterator it = request.env.begin(); it != request.env.end(); it++)
		std::cerr << _YELLOW << '\t' << it->first << " --> " << it->second << _END << std::endl;
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
	std::cerr << std::endl;
	for (size_t n = 0; _env[n]; n++) {
		std::cerr << _CYAN << _env[n] << "$" << std::endl;
	}
	std::cerr << _END;
}

void	Cgi::clear_env() {
	for (int i = 0; _env[i]; i++) {
		free(_env[i]);
		_env[i] = NULL;
	}
	free(_env);
	_env = NULL;
}

int Cgi::run_cgi(request_s &request, std::string& scriptpath) {
	static int testnb = 0;
	std::string inputfile("/tmp/webservin" + ft::inttostring(testnb)),
				outputfile("/tmp/webservout" + ft::inttostring(testnb));
	testnb += 1;
	int				incoming_file,
					outgoing_file;
	pid_t			pid;
	char*			args[2] = {&scriptpath[0], NULL};

	std::cerr << _BOLD _CYAN << "running cgi with uri " << request.uri << std::endl
				<< "and scriptpath " << scriptpath << "." _END << std::endl;

	this->populate_map(request);
	this->map_to_env(request);

	if ((incoming_file = open(inputfile.c_str(), O_CREAT | O_TRUNC | O_RDWR, S_IRWXU)) == -1)
		exit_fatal();
	ssize_t dummy = write(incoming_file, request.body.c_str(), request.body.length()); // Child can read from the other end of this pipe
	(void)dummy;
	std::cout << _CYAN "just wrote a body of size " << dummy << " into the execve.\n" _END;
	if (close(incoming_file) == -1)
		exit_fatal();

	if ((pid = fork()) == -1)
		exit_fatal();

	if (pid == 0) {
		if ((outgoing_file = open(outputfile.c_str(), O_CREAT | O_TRUNC | O_RDWR, S_IRWXU)) == -1)
			exit_fatal();
		if (dup2(outgoing_file, STDOUT_FILENO) == -1 || close(outgoing_file) == -1)
			exit_fatal();
		if ((incoming_file = open(inputfile.c_str(), O_RDONLY, S_IRWXU)) == -1)
			exit_fatal();
		if (dup2(incoming_file, STDIN_FILENO) == -1 || close(incoming_file) == -1)
			exit_fatal();
		if (execve(scriptpath.c_str(), args, _env) == -1)
			std::cerr << "execve: " << strerror(errno) << std::endl;
		exit(EXIT_FAILURE);
	}

	this->clear_env();
	std::cerr << "waiting for child to close\n";
	waitpid(0, NULL, 0);
	std::cerr << "child fucking died lmao\n";
	if ((outgoing_file = open(outputfile.c_str(), O_RDONLY, S_IRWXU)) == -1)
		exit_fatal();
	return (outgoing_file);
}
