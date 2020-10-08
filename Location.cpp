/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Location.cpp                                       :+:    :+:            */
/*                                                     +:+                    */
/*   By: pde-bakk <pde-bakk@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/10/08 14:50:52 by pde-bakk      #+#    #+#                 */
/*   Updated: 2020/10/08 15:01:59 by pde-bakk      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Location.hpp"

Location::Location() {
	
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
	}
}


void	Location::setup(int fd) {
	std::map<std::string, void (Location::*)(const std::string&)> m;
	m["root"] = &Location::setroot;
	m["autoindex"] = &Location::setautoindex;
	m["allow_method"] = &Location::setallow_method;
	m["index"] = &Location::setindex;
	m["cgi"] = &Location::setcgi;
}
