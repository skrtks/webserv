/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Server.hpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: peerdb <peerdb@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/09/29 16:32:46 by peerdb        #+#    #+#                 */
/*   Updated: 2020/10/08 18:07:29 by pde-bakk      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP
# include "webserv.hpp"

class Location;

class Server {
	public:
		Server();
		Server(int fd);
		~Server();
		Server(const Server& x);
		Server& 	operator=(const Server& x);
		void		setport(const std::string& port);
		size_t		getport() const;
		void		sethost(const std::string& host);
		std::string	gethost() const;
		void		setservername(const std::string& servername);
		std::string	getservername() const;
		void		setclientbodysize(const std::string& clientbodysize);
		size_t		getclientbodysize() const;
		void		seterrorpage(const std::string& errorpage);
		std::string	geterrorpage() const;
		void		configurelocation(const std::string& in);
		std::vector<Location> getlocations() const;
		void		setup(int fd);
		bool		getsuccess() const;
		void		clear();
	private:
		size_t	_port,
				_client_body_size;
		std::string _host,
					_server_name,
					_error_page;
		bool	_success;
		int		_fd;
		std::vector<Location> _locations;
};

std::ostream&	operator<<(std::ostream& o, const Server& x);

#endif
