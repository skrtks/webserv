/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Location.cpp                                       :+:    :+:            */
/*                                                     +:+                    */
/*   By: pde-bakk <pde-bakk@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/10/08 14:50:52 by pde-bakk      #+#    #+#                 */
/*   Updated: 2020/10/09 14:27:05 by pde-bakk      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Location.hpp"


Location::Location() {
	this->_success = false;
	this->_location_match = "";
}

Location::Location(std::string& location_match) {
	this->_success = false;
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
	}
	return *this;
}

//setters
void	Location::setroot(const std::string& in) {
	struct stat statstruct;
	this->_root = in;
	if (stat(_root.c_str(), &statstruct) == -1)
		this->_success = false;
}
void	Location::setautoindex(const std::string& in) {
	this->_autoindex = in;
}
void	Location::setallow_method(const std::string& in) {
	this->_allow_method = split(in, " \t\r\n\v\f");
}
void	Location::setindex(const std::string& in) {
	this->_indexes = split(in, " \t\r\n\v\f");
}
void	Location::setcgi(const std::string& in) {
	this->_cgi = split(in, " \t\r\n\v\f");
}

//getters
bool						Location::getsuccess() const {
	return this->_success;
}
std::string					Location::getroot() const {
	return this->_root;
}
std::string					Location::getautoindex() const {
	return this->_autoindex;
}
std::string					Location::getlocationmatch() const {
	return this->_location_match;
}
std::vector<std::string>	Location::getallowmethods() const {
	return this->_allow_method;
}
std::vector<std::string>	Location::getindexes() const {
	return this->_indexes;
}
std::vector<std::string>	Location::getcgi() const {
	return this->_cgi;
}

void	Location::setup(int fd) {
	std::map<std::string, void (Location::*)(const std::string&)> m;
	m["root"] = &Location::setroot;
	m["autoindex"] = &Location::setautoindex;
	m["allow_method"] = &Location::setallow_method;
	m["index"] = &Location::setindex;
	m["cgi"] = &Location::setcgi;
	std::string str;
	this->_success = true;
	
	while (get_next_line(fd, str) > 0) {
		std::string key, value;
		if (is_first_char(str) || str == "") // checks for comments or empty lines
			continue ;
		if (is_first_char(str, '}')) // End of location block
			break ;
		try {
			get_key_value(str, key, value);
			// std::cout << "key = " << key << ", value = " << value << "$" << std::endl;
			(this->*(m.at(key)))(value);
		}
		catch (std::exception& e) {
			std::cerr << "key=\"" << key << "\", exception =\"" << e.what() << "\"." << std::endl;
			this->_success = false;
		}
	}
	if (this->_indexes.size() == 0)
		this->_indexes.push_back("index.html");
	//check if settings correct?
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
