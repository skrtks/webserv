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
#include "Colours.hpp"

Location::Location() {
	this->_location_match = "";
}

Location::Location(std::string& location_match) {
	this->_location_match = location_match;
	this->_autoindex = "off";
}

Location::~Location() {
}

Location::Location(const Location& x) {
	*this = x;
}

Location&	Location::operator=(const Location& x) {
	if (this != &x) {
		this->_root = x._root;
		this->_autoindex = x._autoindex;
		this->_allow_method = x._allow_method;
		this->_indexes = x._indexes;
		this->_cgi = x._cgi;
		this->_location_match = x._location_match;
		this->_error_page = x._error_page;
	}
	return *this;
}

//setters
void	Location::setautoindex(const std::string& in) { this->_autoindex = in; }
void	Location::setallow_method(const std::string& in) { this->_allow_method = ft::split(in, " \t\r\n\v\f"); }
void	Location::setindex(const std::string& in) { this->_indexes = ft::split(in, " \t\r\n\v\f"); }
void	Location::setcgi(const std::string& in) { this->_cgi = ft::split(in, " \t\r\n\v\f"); }
void	Location::seterrorpage(const std::string& in) { this->_error_page = in; }
void	Location::setroot(const std::string& in) {
	struct stat statstruct = {};
	this->_root = in;
	if (stat(_root.c_str(), &statstruct) == -1)
		throw std::runtime_error("location root folder does not exist.");
}

//getters
std::string					Location::getroot() const { return this->_root; }
std::string					Location::getautoindex() const { return this->_autoindex; }
std::string					Location::getlocationmatch() const { return this->_location_match; }
std::vector<std::string>	Location::getallowmethods() const { return this->_allow_method; }
std::vector<std::string>	Location::getindexes() const { return this->_indexes; }
std::vector<std::string>	Location::getcgi() const { return this->_cgi; }
std::string					Location::geterrorpage() const { return this->getroot() + '/' + this->_error_page; }
std::string					Location::getindex() const {
	struct stat statstruct = {};
	std::string filepath;

	for (std::vector<std::string>::const_iterator it = _indexes.begin(); it != _indexes.end(); ++it) {
		filepath = this->_root + '/' + *it;
		if (stat(filepath.c_str(), &statstruct) != -1)
			return (filepath);
	}
	return "";
}

void	Location::setup(int fd) {
	std::map<std::string, void (Location::*)(const std::string&)> m;
	m["root"] = &Location::setroot;
	m["autoindex"] = &Location::setautoindex;
	m["allow_method"] = &Location::setallow_method;
	m["index"] = &Location::setindex;
	m["cgi"] = &Location::setcgi;
	m["error_page"] = &Location::seterrorpage;
	std::string str;
	
	while (ft::get_next_line(fd, str) > 0) {
		std::string key, value;
		if (is_first_char(str) || str.empty()) // checks for comments or empty lines
			continue ;
		if (is_first_char(str, '}')) // End of location block
			break ;
		get_key_value(str, key, value);
//		 std::cout << "key = " << key << ", value = " << value << "$" << std::endl;
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
	o	<< "Location block \"" << x.getlocationmatch() << "\":" << std::endl
		<< "\troot folder: \"" << x.getroot() << "\"" << std::endl
		<< "\tautoindex is: \"" << x.getautoindex() << "\"" << std::endl;
	o	<< "\tallowed methods:";
	v = x.getallowmethods();
	for (size_t i = 0; i < v.size(); i++)
		o << " \"" << v[i] << "\"";
	o << std::endl;
	o	<< "\tindexes:";
	v = x.getindexes();
	for (size_t i = 0; i < v.size(); i++)
		o << " \"" << v[i] << "\"";
	o << std::endl;
	o	<< "\tcgi:";
	v = x.getcgi();
	for (size_t i = 0; i < v.size(); i++)
		o << " \"" << v[i] << "\"";
	o << std::endl;
	return o;
}
