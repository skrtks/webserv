/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Server.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: pde-bakk <pde-bakk@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/10/07 12:57:25 by pde-bakk      #+#    #+#                 */
/*   Updated: 2020/11/10 12:11:31 by peerdb        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "libftGnl.hpp"
#include <sys/stat.h>
#include <fstream>
#include "Base64.hpp"
#include "Colours.hpp"

Server::Server() : _port(80), _client_body_size(1000000),
		_host("0.0.0.0"), _error_page("error.html"), _404_page("404.html"), 
		_index("index.html"), _root("htmlfiles"), 
		_auth_basic_realm("Access to the staging site"), _htpasswd_path(), _fd(),
		_socketFd() {
}

Server::Server(int fd) : _port(80), _client_body_size(1000000),
		_host("0.0.0.0"), _error_page("error.html"), _404_page("404.html"),
		_index("index.html"), _root("htmlfiles"),
		_auth_basic_realm("Access to the staging site"), _htpasswd_path(),
		_socketFd() {
	this->_fd = fd;
}

Server::~Server() {
}

Server::Server(const Server& x) : _port(), _client_body_size(), _fd(), _socketFd() {
	*this = x;
}

Server&	Server::operator=(const Server& x) {
	if (this != &x) {
		this->_port = x._port;
		this->_host = x._host;
		this->_server_name = x._server_name;
		this->_client_body_size = x._client_body_size;
		this->_error_page = x._error_page;
		this->_404_page = x._404_page;
		this->_index = x._index;
		this->_root = x._root;
		this->_auth_basic_realm = x._auth_basic_realm;
		this->_htpasswd_path = x._htpasswd_path;
		this->_locations = x._locations;
		this->_socketFd = x._socketFd;
		this->_base_env = x._base_env;
		this->_auth_basic_realm = x._auth_basic_realm;
		this->_htpasswd_path = x._htpasswd_path;
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
	this->_client_body_size = ft_atol(clientbodysize.c_str());
	if (clientbodysize[clientbodysize.find_first_not_of("1234567890")] == 'M')
		this->_client_body_size *= 1000000;
}

std::string	Server::geterrorpage() const {
	return this->_error_page;
}

void	Server::seterrorpage(const std::string& errorpage) {
	this->_error_page = errorpage;
}

std::string	Server::get404page() const {
	return this->_404_page;
}

void	Server::set404page(const std::string& page) {
	this->_404_page = page;
}
void	Server::setauth_basic_realm(const std::string &realm) {
	this->_auth_basic_realm = realm;
}

std::string	Server::getauthbasicrealm() const {
	return this->_auth_basic_realm;
}

void	Server::sethtpasswdpath(const std::string &path) {
	struct stat statstruct = {};
	if (stat(path.c_str(), &statstruct) == -1)
		return ;

	this->_htpasswd_path = path;
	std::ifstream ifs;
	ifs.open(_htpasswd_path.c_str());
	if (ifs.bad())
		return ;
	std::cerr << "we opened " << _htpasswd_path.c_str() << std::endl;
	std::string line;
	while (std::getline(ifs, line)) {
		std::string user, pass;
//		std::cerr << "line = " << line << std::endl;
		get_key_value(line, user, pass, ":");
		this->_loginfo[user] = pass;
		std::cerr << _CYAN "added '" << user << ':' << pass << "' to _loginfo map\n" << _END;
	}
}

std::string	Server::gethtpasswdpath() const {
	return this->_htpasswd_path;
}

std::map<std::string, std::string> Server::getbaseenv() const {
	return this->_base_env;
}

void	Server::configurelocation(const std::string& in) {
	std::vector<std::string> v = ft::split(in, " \t\r\n\v\f");
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
	m["auth_basic"] = &Server::setauth_basic_realm;
	m["auth_basic_user_file"] = &Server::sethtpasswdpath;
	m["server_name"] = &Server::setservername;
	m["LIMIT_CLIENT_BODY_SIZE"] = &Server::setclientbodysize;
	m["DEFAULT_ERROR_PAGE"] = &Server::seterrorpage;
	m["location"] = &Server::configurelocation;
	std::string str;

	while (ft::get_next_line(fd, str) > 0) {
		std::string key, value;
		if (is_first_char(str) || is_first_char(str, ';') || str.empty()) //checks for comment char #
			continue ;
		if (is_first_char(str, '}')) // End of server block
			break ;
		get_key_value(str, key, value);
//		 std::cout << "key = " << key << ", value = " << value << "$" << std::endl;
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

bool Server::getmatch(const std::string &username, const std::string &passwd) {
	std::map<std::string, std::string>::const_iterator it = this->_loginfo.find(username);
	
	std::cerr << _CYAN "trying to log in with " << username << " and " << base64_decode(passwd) << _END << std::endl;
	if (it != _loginfo.end() && it->second == base64_decode(passwd) )
		return true;
	std::cerr << "finding matching login info failed\n";
	return false;
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
