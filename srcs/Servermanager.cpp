/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Servermanager.cpp                                  :+:    :+:            */
/*                                                     +:+                    */
/*   By: pde-bakk <pde-bakk@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/10/07 20:37:58 by pde-bakk      #+#    #+#                 */
/*   Updated: 2020/11/23 17:17:20 by pde-bakk      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Servermanager.hpp"

Servermanager::Servermanager() : _servers() {
}

Servermanager::~Servermanager() {
}

Servermanager::Servermanager(const Servermanager& x) {
	*this = x;
}

Servermanager& Servermanager::operator=(const Servermanager& x) {
	if (this != &x) {
		this->_servers = x._servers;
	}
	return *this;
}

Servermanager& Servermanager::operator+=(const Server& add) {
	this->_servers.push_back(add);
	return *this;
}

Server	Servermanager::operator[](const size_t& n) const {
	return this->_servers[n];
}

size_t	Servermanager::size() const {
	return this->_servers.size();
}

void	Servermanager::clear() {
	this->_servers.clear();
}

const std::vector<Server>& Servermanager::getServers() const {
	return _servers;
}
