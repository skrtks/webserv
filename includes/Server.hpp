/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Server.hpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: peerdb <peerdb@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/09/29 16:32:46 by peerdb        #+#    #+#                 */
/*   Updated: 2020/10/13 16:28:59 by pde-bakk      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

#include "Location.hpp"

class Server {
	public:
		//coplien form
		Server();
		Server(int fd);
		~Server();
		Server(const Server& x);
		Server& 	operator=(const Server& x);

		//setters
		void		setport(const std::string& port);
		void		sethost(const std::string& host);
		void		setservername(const std::string& servername);
		void		setclientbodysize(const std::string& clientbodysize);
		void		seterrorpage(const std::string& errorpage);
		void		setSocketFd(int socketFd);
	
		//getters
		size_t					getport() const;
		std::string				gethost() const;
		std::string				getservername() const;
		size_t					getclientbodysize() const;
		std::string				geterrorpage() const;
		std::vector<Location> 	getlocations() const;
		int						getSocketFd() const;
	
		void		configurelocation(const std::string& in);
		void		setup(int fd);
private:
		size_t		_port,
					_client_body_size;
		std::string _host,
					_server_name,
					_error_page;
		int			_fd, _socketFd;
		std::vector<Location> _locations;
};

std::ostream&	operator<<(std::ostream& o, const Server& x);

#endif
