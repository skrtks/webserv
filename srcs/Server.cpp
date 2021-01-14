/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Server.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: pde-bakk <pde-bakk@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/10/07 12:57:25 by pde-bakk      #+#    #+#                 */
/*   Updated: 2020/11/23 17:18:11 by pde-bakk      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include	"Base64.hpp"
#include	"Server.hpp"
#include	"libftGnl.hpp"
#include	<cerrno>
#include	<sys/stat.h>
//# include <fcntl.h>
//#include <zconf.h>
//#include <sys/select.h>
//#include <netinet/tcp.h>

Server::Server() : _port(80), _maxfilesize(1000000),
		_host("0.0.0.0"), _error_page("error.html"),
		_root("htmlfiles"),
		_auth_basic_realm(), _htpasswd_path(),
		_fd(), _socketFd(), addr() {
}

Server::Server(int fd) : _port(80), _maxfilesize(1000000),
		_host("0.0.0.0"), _error_page("error.html"),
		_root("htmlfiles"), _auth_basic_realm(), _htpasswd_path(),
		_socketFd(), addr() {
	this->_fd = fd;
}

Server::~Server() {
	close(_socketFd);
}

Server::Server(const Server& x) : _port(), _maxfilesize(), _fd(), _socketFd(), addr() {
	*this = x;
}

Server&	Server::operator=(const Server& x) {
	if (this != &x) {
		this->_port = x._port;
		this->_host = x._host;
		this->_server_name = x._server_name;
		this->_maxfilesize = x._maxfilesize;
		this->_error_page = x._error_page;
		this->_indexes = x._indexes;
		this->_root = x._root;
		this->_auth_basic_realm = x._auth_basic_realm;
		this->_htpasswd_path = x._htpasswd_path;
		this->_locations = x._locations;
		this->_socketFd = x._socketFd;
		this->addr = x.addr;
		this->_auth_basic_realm = x._auth_basic_realm;
		this->_htpasswd_path = x._htpasswd_path;
		this->_loginfo = x._loginfo;
		this->_connections = x._connections;
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
	struct stat statstruct = {};
	std::string filepath;
	for (std::vector<std::string>::const_iterator it = _indexes.begin(); it != _indexes.end(); ++it) {
		filepath = this->_root + *it;
		if (stat(filepath.c_str(), &statstruct) != -1)
			return *it;
	}
	return this->_error_page;
}

void	Server::setindexes(const std::string& index) {
	this->_indexes = ft::split(index, " \t\r\n\v\f");
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
	this->_server_name = servername.substr(servername.find_first_not_of(" \t\r\n\f\v"), servername.find_last_not_of(" \t\r\n\f\v") - servername.find_first_not_of(" \t\r\n\f\v") + 1);
}

long int	Server::getmaxfilesize() const {
	return this->_maxfilesize;
}

void	Server::setmaxfilesize(const std::string& clientbodysize) {
	this->_maxfilesize = ft_atol(clientbodysize.c_str());
	if (clientbodysize[clientbodysize.find_first_not_of("1234567890")] == 'M')
		this->_maxfilesize *= 1000000;
}

std::string	Server::geterrorpage() const {
	return this->getroot() + '/' + this->_error_page;
}

void	Server::seterrorpage(const std::string& errorpage) {
	this->_error_page = errorpage;
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
	int htpasswd_fd = open(this->_htpasswd_path.c_str(), O_RDONLY);
	if (htpasswd_fd < 0)
		return;
	std::string line;
	while (ft::get_next_line(htpasswd_fd, line)) {
		std::string user, pass;
		get_key_value(line, user, pass, ":");
		this->_loginfo[user] = pass;
	}
}

std::string	Server::gethtpasswdpath() const {
	return this->_htpasswd_path;
}

void	Server::configurelocation(const std::string& in) {
	std::vector<std::string> v = ft::split(in, " \t\r\n\v\f");
	Location	loc(v[0]);
	loc.setup(this->_fd);
	this->_locations.push_back(loc);
}

std::vector<Location>	Server::getlocations() const {
	return this->_locations;
}

void	Server::setup(int fd) {
	this->_fd = fd;
	std::map<std::string, void (Server::*)(const std::string&)> m;
	m["port"] = &Server::setport;
	m["host"] = &Server::sethost;
	m["autoindex"] = &Server::setautoindex;
	m["index"] = &Server::setindexes;
	m["root"] = &Server::setroot;
	m["auth_basic"] = &Server::setauth_basic_realm;
	m["auth_basic_user_file"] = &Server::sethtpasswdpath;
	m["server_name"] = &Server::setservername;
	m["max_filesize"] = &Server::setmaxfilesize;
	m["error_page"] = &Server::seterrorpage;
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
	if (_port <= 0 || _host.empty() || _maxfilesize <= 0 || _error_page.empty() || _server_name.empty())
		throw std::runtime_error("invalid setting in server block");
}

int Server::getSocketFd() const {
	return _socketFd;
}

void Server::setautoindex(const std::string& ai) {
	this->_autoindex = ai;
}

std::string Server::getautoindex() const {
	return this->_autoindex;
}

Location Server::matchlocation(const std::string &uri) const {
	size_t		n;
	Location	out;

	for (std::vector<Location>::const_iterator it = this->_locations.begin(); it != this->_locations.end(); ++it) {
		n = it->getlocationmatch().length();
		if (it->getlocationmatch()[n - 1] == '/' && n > 1)
			n -= 1;
		if (n >= out.getlocationmatch().length() && it->getlocationmatch().compare(0, n, uri, 0, n) == 0)
			out = *it;
	}
	out.addServerInfo(this->_root, this->_autoindex, this->_indexes, this->_error_page);
	return (out);
}

std::string	Server::getfilepath(const std::string& uri) const {
	Location loca = this->matchlocation(uri);

	std::string	TrimmedUri(uri),
				filepath(loca.getroot());
	TrimmedUri.erase(0, loca._location_match.length());
	if (filepath[filepath.length() - 1] != '/' && TrimmedUri[0] != '/') // I wanna use .front() and .back() but they're C++11 :sadge:
		filepath += '/';
	filepath += TrimmedUri;
	return (filepath);
}

int Server::getpage(const std::string &uri, std::map<headerType, std::string>& headervals, int& statuscode) const {
	struct stat statstruct = {};
	int fd = -1;
	Location loca = this->matchlocation(uri);
	std::string filepath = this->getfilepath(uri);

	if (stat(filepath.c_str(), &statstruct) != -1) {
		if (S_ISDIR(statstruct.st_mode)) {
			if (filepath[filepath.length() - 1] != '/')
				filepath += '/';
			filepath += loca.getindex();
		}
		if (!filepath.empty())
			fd = open(filepath.c_str(), O_RDONLY);
	}
	if (fd == -1) {
		filepath = loca.geterrorpage();
		fd = open(filepath.c_str(), O_RDONLY);
		statuscode = 404;
	}
 	headervals[CONTENT_LOCATION] = filepath;
	return (fd);
}

bool Server::getmatch(const std::string& username, const std::string& passwd) {
	std::map<std::string, std::string>::const_iterator it = this->_loginfo.find(username);

	return ( it != _loginfo.end() && passwd == base64_decode(it->second) );
}

bool Server::isExtensionAllowed(const std::string& uri) const {
	std::string extension = ft::getextension(uri);
	std::vector<std::string> allowed_extensions = matchlocation(uri).getcgiallowedextensions();

	for (std::vector<std::string>::const_iterator it = allowed_extensions.begin(); it != allowed_extensions.end(); ++it) {
		if (extension == *it)
			return (true);
	}
	return (false);
}

void Server::startListening() {
	bzero(&addr, sizeof(addr));

	if ((_socketFd = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
		std::cerr << _RED _BOLD "Error setting server socket\n" _END;
		throw std::runtime_error(strerror(errno));
	}
	int x = 1;
	if (setsockopt(_socketFd, SOL_SOCKET, SO_REUSEPORT, &x, sizeof(x)) == -1) {
		std::cerr << _RED _BOLD "Error setting server socket options\n" _END;
		throw std::runtime_error(strerror(errno));
	}
	// Fill struct with info about port and ip
	addr.sin_family = AF_INET; // ipv4
	if (gethost() == "localhost") {
		addr.sin_addr.s_addr = INADDR_ANY; // choose local ip for me
	}
	else {
		addr.sin_addr.s_addr = inet_addr(gethost().c_str());
	}
	addr.sin_port = htons(getport());

	// Associate the socket with a port and ip
	for (int i = 0; i < 6; ++i) {
		if ((bind(this->_socketFd, (struct sockaddr *) &addr, sizeof(addr))) == -1) {
			std::cerr << "Cannot bind (" << errno << " " << strerror(errno) << ")" << std::endl;
			if (i == 5)
				throw std::runtime_error(strerror(errno));
		} else {
			break;
		}
	}
	// Start listening for connections on port set in sFd, max BACKLOG waiting connections
	if (listen(this->_socketFd, BACKLOG) == -1) {
		std::cerr << _RED _BOLD "Error listening to server socket\n" _END;
		throw std::runtime_error(strerror(errno));
	}

	if (fcntl(this->_socketFd, F_SETFL, O_NONBLOCK) == -1) {
		std::cerr << _RED _BOLD "Error setting server socket to be nonblocking\n" _END;
		throw std::runtime_error(strerror(errno));
	}
}

int Server::addConnection() {
	Client* newClient = new Client(this);
	this->_connections.push_back(newClient);
	return newClient->fd;
}

void Server::showclients(const fd_set& readfds, const fd_set& writefds) {
	for (std::vector<Client*>::const_iterator it = this->_connections.begin(); it != this->_connections.end(); ++it) {
		std::cerr << _CYAN " -- We have a client with fd " << (*it)->fd << " at " << (*it)->ipaddress << ".\n";
		std::cerr << "It is " << (FD_ISSET((*it)->fd, &readfds) ? "" : "not") << " readable.\n";
		std::cerr << "It is " << (FD_ISSET((*it)->fd, &writefds) ? "" : "not") << " writeable.\n";
	}
}

std::ostream& operator<<( std::ostream& o, const Server& x) {
	o << x.getservername() <<  " is listening on: " << x.gethost() << ":" << x.getport() << std::endl;
	o << "Default root folder: " << x.getroot() << std::endl;
	o << "Default index page: " << x.getindex() << std::endl;
	o << "error page: " << x.geterrorpage() << std::endl;
	o << "client body limit: " << x.getmaxfilesize() << std::endl << std::endl;
	std::vector<Location> v = x.getlocations();
	for (size_t i = 0; i < v.size(); i++) {
		o << v[i];
	}
	o << std::endl;
	return (o);
}

