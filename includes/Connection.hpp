/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Connection.hpp                                     :+:    :+:            */
/*                                                     +:+                    */
/*   By: sam <sam@student.codam.nl>                   +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/10/03 15:26:41 by sam           #+#    #+#                 */
/*   Updated: 2020/11/28 20:05:15 by tuperera      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONNECTION_HPP
#define CONNECTION_HPP
#include "Server.hpp"
#define BUFLEN 8192

class Connection {
	int _socketFd;
	fd_set	_readFds, // temp file descriptor list for select()
			_writeFds,
			_readFdsBak, // temp file descriptor list for select()
			_writeFdsBak;
	std::set<int>	_allConnections;
	std::vector<Server*> _servers;
	char* _configPath;

public:
	Connection();
	explicit Connection(char* configPath);
	Connection(const Connection &obj);
	Connection& operator= (const Connection &obj);
	virtual ~Connection();

	void startListening();

	void startServer();
	void loadConfiguration();
	void handleCLI();
	void stopServer();
	bool	checkIfEnded(const std::string& request);
	int		getMaxFD();
};

#endif //CONNECTION_HPP
