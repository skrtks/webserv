/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Location.cpp                                       :+:    :+:            */
/*                                                     +:+                    */
/*   By: pde-bakk <pde-bakk@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/10/08 14:50:52 by pde-bakk      #+#    #+#                 */
/*   Updated: 2020/10/15 18:23:06 by pde-bakk      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Location.hpp"
#include <sys/stat.h>
#include "libftGnl.hpp"
#include <climits>

Location::Location() : _maxBody(LONG_MAX), _default_cgi_path() {
	this->_location_match = "";
}

Location::Location(std::string& location_match) : _maxBody(LONG_MAX), _default_cgi_path() {
	this->_location_match = location_match;
	this->_autoindex = "off";
}

Location::~Location() {
}

Location::Location(const Location& x) : _maxBody(LONG_MAX) {
	*this = x;
}

Location&	Location::operator=(const Location& x) {
	if (this != &x) {
		this->_root = x._root;
		this->_autoindex = x._autoindex;
		this->_allow_method = x._allow_method;
		this->_indexes = x._indexes;
		this->_cgi_allowed_extensions = x._cgi_allowed_extensions;
		this->_location_match = x._location_match;
		this->_error_page = x._error_page;
		this->_maxBody = x._maxBody;
		this->_default_cgi_path = x._default_cgi_path;
		this->_php_cgi = x._php_cgi;
	}
	return *this;
}

std::vector<e_method>	StringToMethod(const std::vector<std::string>& in) {
	std::vector<e_method>	out;
	for (std::vector<std::string>::const_iterator it = in.begin(); it != in.end(); it++) {
		if (*it == "HEAD")
			out.push_back(HEAD);
		else if (*it == "GET")
			out.push_back(GET);
		else if (*it == "POST")
			out.push_back(POST);
		else if (*it == "PUT")
			out.push_back(PUT);
		else throw std::runtime_error("invalid method");
	}
	return (out);
}

//setters
void	Location::setautoindex(const std::string& in) { this->_autoindex = in; }
void	Location::setallow_method(const std::string& in) { this->_allow_method = StringToMethod(ft::split(in, " \t\r\n\v\f")); }
void	Location::setindex(const std::string& in) { this->_indexes = ft::split(in, " \t\r\n\v\f"); }
void	Location::setcgiallowedextensions(const std::string& in) { this->_cgi_allowed_extensions = ft::split(in, " \t\r\n\v\f"); }
void	Location::seterrorpage(const std::string& in) { this->_error_page = in; }
void	Location::setmaxbody(const std::string& in) { this->_maxBody = ft_atoi(in.c_str()); }
void	Location::setdefaultcgipath(const std::string& in) {
	struct stat statstruct = {};
	std::string	filepath = this->_root + '/' + in;
	if (stat(filepath.c_str(), &statstruct) != -1)
		this->_default_cgi_path = in;
}
void	Location::setroot(const std::string& in) {
	struct stat statstruct = {};
	this->_root = in;
	if (stat(_root.c_str(), &statstruct) == -1)
		throw std::runtime_error("location root folder does not exist.");
}


void Location::setphpcgipath(const std::string& in) {
	struct stat statstruct = {};
	this->_php_cgi = in;
	if (stat(_root.c_str(), &statstruct) == -1)
		throw std::runtime_error("php-cgi executable path does not exist.");
}

//getters
std::string					Location::getroot() const { return this->_root; }
std::string					Location::getautoindex() const { return this->_autoindex; }
std::string					Location::getlocationmatch() const { return this->_location_match; }
std::vector<std::string>	Location::getindexes() const { return this->_indexes; }
std::vector<std::string>	Location::getcgiallowedextensions() const { return this->_cgi_allowed_extensions; }
std::string					Location::geterrorpage() const { return this->getroot() + '/' + this->_error_page; }
long unsigned int			Location::getmaxbody() const { return this->_maxBody; }
std::string					Location::getindex() const {
	struct stat statstruct = {};
	for (size_t i = 0; i < this->_indexes.size(); i++) {
		std::string check = this->_root + '/' + this->_indexes[i];
		std::cerr << "check whether this index file exists: '" << check << "'\n";
		if (stat(check.c_str(), &statstruct) != -1) {
			std::cerr << "it does!\n";
			return this->_indexes[i];
		}
	}
	return "";
}
std::string					Location::getdefaultcgipath() const { return this->_default_cgi_path; }
std::string					Location::getphpcgipath() const { return this->_php_cgi; }
std::string					Location::getallowedmethods() const {
	std::string ret("Allow:");
	for (size_t i = 0; i < this->_allow_method.size(); ++i) {
		if (i > 0)
			ret += ',';
		ret += " " + methodAsString(this->_allow_method[i]);
	}
	return (ret);
}
bool		Location::checkifMethodAllowed(const e_method& meth) const {
	for (std::vector<e_method>::const_iterator it = this->_allow_method.begin(); it != this->_allow_method.end(); ++it)
		if (*it == meth)
			return true;
	return false;
}

void	Location::setup(int fd) {
	std::map<std::string, void (Location::*)(const std::string&)> m;
	m["root"] = &Location::setroot;
	m["autoindex"] = &Location::setautoindex;
	m["allow_method"] = &Location::setallow_method;
	m["index"] = &Location::setindex;
	m["ext"] = &Location::setcgiallowedextensions;
	m["error_page"] = &Location::seterrorpage;
	m["maxBody"] = &Location::setmaxbody;
	m["default_cgi"] = &Location::setdefaultcgipath;
	m["php-cgi"] = &Location::setphpcgipath;
	std::string str;
	
	while (ft::get_next_line(fd, str) > 0) {
		std::string key, value;
		if (is_first_char(str) || str.empty()) // checks for comments or empty lines
			continue ;
		if (is_first_char(str, '}')) // End of location block
			break ;
		get_key_value(str, key, value);
		if (!m.count(key))
			std::cerr <<_RED _BOLD "Unable to parse key '" << key << "' in Location block " << this->getlocationmatch() << _END << std::endl;
		(this->*(m.at(key)))(value);
	}
	if (this->_indexes.empty())
		this->_indexes.push_back("index.html");
}

void	Location::addServerInfo(const std::string& root, const std::string& autoindex,
							 const std::vector<std::string>& indexes, const std::string& errorpage) {
	if (this->_root.empty())
		this->_root = root;
	if (this->_autoindex.empty())
		this->_autoindex = autoindex;
	if (this->_indexes.empty())
		this->_indexes = indexes;
	if (this->_error_page.empty())
		this->_error_page = errorpage;
}

std::ostream&	operator<<(std::ostream& o, const Location& x) {
	std::vector<std::string> v;
	std::vector<e_method>	meths;
	o	<< "Location block \"" << x.getlocationmatch() << "\":" << std::endl
		<< "\troot folder: \"" << x.getroot() << "\"" << std::endl
		<< "\tautoindex is: \"" << x.getautoindex() << "\"" << std::endl;
	o	<< '\t' << x.getallowedmethods() << std::endl;
	o	<< "\tindexes:";
	v = x.getindexes();
	for (size_t i = 0; i < v.size(); i++)
		o << " \"" << v[i] << "\"";
	o << std::endl;
	o	<< "\tcgi:";
	v = x.getcgiallowedextensions();
	for (size_t i = 0; i < v.size(); i++)
		o << " \"" << v[i] << "\"";
	o << std::endl;
	return o;
}
