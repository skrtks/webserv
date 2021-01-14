//
// Created by peerdb on 02-12-20.
//

#include "Cgi.hpp"
#include <sys/wait.h>
#include <cerrno>
#include "libftGnl.hpp"
#include "Colours.hpp"
#include "Server.hpp"

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

void Cgi::populate_map(request_s &req, const std::string& OriginalUri, bool redirect_status) {
	int split_path = req.uri.find_first_of('/', req.uri.find_first_of('.') );
	char buf[500];
	std::string	realpath = getcwd(buf, 500);
	this->_m["AUTH_TYPE"] = req.headers[AUTHORIZATION];
	this->_m["CONTENT_LENGTH"] = ft::inttostring(req.body.size());
	this->_m["CONTENT_TYPE"] = req.headers[CONTENT_TYPE];
	this->_m["GATEWAY_INTERFACE"] = "CGI/1.1";
	this->_m["PATH_INFO"] = OriginalUri + req.cgiparams;
	this->_m["PATH_TRANSLATED"] = realpath + this->_m["PATH_INFO"];
	this->_m["QUERY_STRING"] = req.cgiparams;
	this->_m["REMOTE_ADDR"] = req.server->gethost();
	this->_m["REMOTE_IDENT"] = "";
	this->_m["REMOTE_USER"] = req.headers[REMOTE_USER];
	this->_m["REQUEST_METHOD"] = req.MethodToSTring();
	this->_m["REQUEST_URI"] = OriginalUri;
	this->_m["SCRIPT_FILENAME"] = req.uri.substr(1, split_path - 1 );
	this->_m["SCRIPT_NAME"] = req.uri.substr(1, split_path - 1 );
	this->_m["SERVER_NAME"] = req.server->getservername();
	this->_m["SERVER_PORT"] = std::string(ft::inttostring(req.server->getport()));
	this->_m["SERVER_PROTOCOL"] = "HTTP/1.1";
	this->_m["SERVER_SOFTWARE"] = "HTTP 1.1";
	if (redirect_status)
		this->_m["REDIRECT_STATUS"] = "true";
}

void Cgi::map_to_env(request_s& request) {
	int i = 0;
	this->_m.insert(request.env.begin(), request.env.end());
	this->_env = (char**) ft_calloc(this->_m.size() + 1, sizeof(char*));
	if (!_env)
		exit_fatal();

	for (std::map<std::string, std::string>::const_iterator it = _m.begin(); it != _m.end(); it++) {
		std::string tmp = it->first + "=" + it->second;
		std::cerr << _YELLOW "\t" << tmp << _END << std::endl;
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

#include <sys/stat.h>

int Cgi::run_cgi(request_s &request, std::string& scriptpath, const std::string& OriginalUri) {
	int				incoming_file,
					outgoing_file;
	pid_t			pid;
	bool			redirect_status = false;
	char*			args[3] = {&scriptpath[0], NULL, NULL};

	std::string phpcgipath = request.server->matchlocation(OriginalUri).getphpcgipath();
	if (!phpcgipath.empty()) {
		args[0] = ft_strdup(phpcgipath.c_str());
		args[1] = ft_strdup(scriptpath.c_str());
		redirect_status = true;
	}

	this->populate_map(request, OriginalUri, redirect_status);
	this->map_to_env(request);

	if ((incoming_file = open("/tmp/webservin.txt", O_CREAT | O_TRUNC | O_RDWR, S_IRWXU)) == -1)
		exit_fatal();
	ssize_t dummy = write(incoming_file, request.body.c_str(), request.body.length()); // Child can read from the other end of this pipe
	(void)dummy;
	if (close(incoming_file) == -1)
		exit_fatal();
	if ((pid = fork()) == -1)
		exit_fatal();

	if (pid == 0) {
		std::cerr << args[0] << " " << args[1] << (args[2] == NULL ? " NULL" : args[2]) << std::endl;
		if ((outgoing_file = open("/tmp/webservout.txt", O_CREAT | O_TRUNC | O_RDWR, S_IRWXU)) == -1)
			exit_fatal();
		if (dup2(outgoing_file, STDOUT_FILENO) == -1 || close(outgoing_file) == -1)
			exit_fatal();
		if ((incoming_file = open("/tmp/webservin.txt", O_RDONLY, S_IRWXU)) == -1)
			exit_fatal();
		if (dup2(incoming_file, STDIN_FILENO) == -1 || close(incoming_file) == -1)
			exit_fatal();
		if (execve(args[0], args, _env) == -1)
			std::cerr << "execve: " << strerror(errno) << std::endl;
		exit(EXIT_FAILURE);
	}

	this->clear_env();
	waitpid(0, NULL, 0);
	if ((outgoing_file = open("/tmp/webservout.txt", O_RDONLY, S_IRWXU)) == -1)
		exit_fatal();
	if (!phpcgipath.empty()) {
		free(args[0]);
		free(args[1]);
	}
	return (outgoing_file);
}
