/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Server.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: pde-bakk <pde-bakk@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/10/07 12:57:25 by pde-bakk      #+#    #+#                 */
/*   Updated: 2020/10/17 11:19:07 by peerdb        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "libftGnl.hpp"

Server::Server() : _port(80), _client_body_size(1000000),
		_host("0.0.0.0"), _error_page("error.html"), _index("index.html"), _root("htmlfiles") {
}

Server::Server(int fd) : _port(80), _client_body_size(1000000),
		_host("0.0.0.0"), _error_page("error.html"), _index("index.html"), _root("htmlfiles") {
	this->_fd = fd;
}

Server::~Server() {
}

Server::Server(const Server& x) {
	*this = x;
}

Server&	Server::operator=(const Server& x) {
	if (this != &x) {
		this->_port = x._port;
		this->_host = x._host;
		this->_server_name = x._server_name;
		this->_client_body_size = x._client_body_size;
		this->_error_page = x._error_page;
		this->_index = x._index;
		this->_root = x._root;
		this->_locations = x._locations;
		this->_socketFd = x._socketFd;
		this->_base_env = x._base_env;
	}
	return *this;
}

size_t	Server::getport() const {
	return this->_port;
}

void	Server::setport(const std::string& port) {
	this->_port = ft_atoi(port.c_str());
}

std::string	Server::gethost() const {
	return this->_host;
}

void	Server::sethost(const std::string& host) {
	this->_host = host;
}

std::string	Server::getindex() const {
	return this->_index;
}

void	Server::setindex(const std::string& index) {
	this->_index = index;
}

std::string	Server::getroot() const {
	return this->_root;
}

void	Server::setroot(const std::string& root) {
	this->_root = root;
}

std::string	Server::getservername() const {
	return this->_server_name;
}

void	Server::setservername(const std::string& servername) {
	this->_server_name = servername;
}

long int	Server::getclientbodysize() const {
	return this->_client_body_size;
}

void	Server::setclientbodysize(const std::string& clientbodysize) {
	this->_client_body_size = static_cast<long int>(ft_atoll(clientbodysize.c_str(), 10));
	size_t i = clientbodysize.find_first_not_of("1234567890");
	if (clientbodysize[i] == 'M')
		this->_client_body_size *= 1000000;
}

std::string	Server::geterrorpage() const {
	return this->_error_page;
}

void	Server::seterrorpage(const std::string& errorpage) {
	this->_error_page = errorpage;
}

std::map<std::string, std::string> Server::getbaseenv() const {
	return this->_base_env;
}

void	Server::configurelocation(const std::string& in) {
	std::vector<std::string> v = ft::split(in, " \t\r\n\v\f\0");
	Location	loc(v[0]);
	loc.setup(this->_fd);
	// if succesful
	this->_locations.push_back(loc);
}

std::vector<Location>	Server::getlocations() const {
	return this->_locations;
}

void	Server::create_base_env() {
	this->_base_env["AUTH_TYPE"] = "";
	this->_base_env["CONTENT_LENGTH"] = "";
	this->_base_env["CONTENT_TYPE"] = "";
	this->_base_env["GATEWAY_INTERFACE"] = "CGI/1.1";
	this->_base_env["PATH_INFO"] = "";
	this->_base_env["PATH_TRANSLATED"] = "";
	this->_base_env["QUERY_STRING"] = "";
	this->_base_env["REMOTE_ADDR"] = "";
	this->_base_env["REMOTE_IDENT"] = "";
	this->_base_env["REMOTE_USER"] = "";
	this->_base_env["REQUEST_METHOD"] = "";
	this->_base_env["REQUEST_URI"] = "";
	this->_base_env["SCRIPT_NAME"] = "";
	this->_base_env["SERVER_NAME"] = this->getservername();
	this->_base_env["SERVER_PORT"] = std::string(ft::inttostring(this->getport()));
	this->_base_env["SERVER_PROTOCOL"] = "HTTP/1.1";
	this->_base_env["SERVER_SOFTWARE"] = "HTTP 1.1";
}

void	Server::setup(int fd) {
	this->_fd = fd;
	std::map<std::string, void (Server::*)(const std::string&)> m;
	m["port"] = &Server::setport;
	m["host"] = &Server::sethost;
	m["index"] = &Server::setindex;
	m["root"] = &Server::setroot;
	m["server_name"] = &Server::setservername;
	m["LIMIT_CLIENT_BODY_SIZE"] = &Server::setclientbodysize;
	m["DEFAULT_ERROR_PAGE"] = &Server::seterrorpage;
	m["location"] = &Server::configurelocation;
	std::string str;

	while (ft::get_next_line(fd, str) > 0) {
		std::string key, value;
		if (is_first_char(str) || str.empty()) //checks for comment char #
			continue ;
		if (is_first_char(str, '}')) // End of server block
			break ;
		get_key_value(str, key, value);
		// std::cout << "key = " << key << ", value = " << value << "$" << std::endl;
		(this->*(m.at(key)))(value); // (this->*(m[key]))(value);
	}
	this->create_base_env();
	if (_port <= 0 || _host.empty() || _client_body_size <= 0 || _error_page.empty() || _server_name.empty())
		throw std::runtime_error("invalid setting in server block");
}

int Server::getSocketFd() const {
	return _socketFd;
}

void Server::setSocketFd(int socketFd) {
	_socketFd = socketFd;
}

std::ostream& operator<<( std::ostream& o, const Server& x) {
	o << x.getservername() <<  " is listening on: " << x.gethost() << ":" << x.getport() << std::endl;
	o << "Default root folder: " << x.getroot() << std::endl;
	o << "Default index page: " << x.getindex() << std::endl;
	o << "error page: " << x.geterrorpage() << std::endl;
	o << "client body limit: " << x.getclientbodysize() << std::endl << std::endl;
	std::vector<Location> v = x.getlocations();
	for (size_t i = 0; i < v.size(); i++) {
		o << v[i];
	}
	o << std::endl;
	return o;
}
