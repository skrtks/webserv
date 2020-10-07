/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Server.hpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: peerdb <peerdb@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/09/29 16:32:46 by peerdb        #+#    #+#                 */
/*   Updated: 2020/10/07 18:01:36 by pde-bakk      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP
# include "webserv.hpp"
# include <cstddef>
# include <string>
# include <map>
# include "getnextline/get_next_line.hpp"
enum E {
	e1,
	e2,
	e3,
	e4,
	e5
};

class Server {
	public:
		Server();
		~Server();
		Server(const Server& x);
		Server& operator=(const Server& x);
		void	setport(const std::string& port);
		size_t	getport() const;
		void	sethost(const std::string& host);
		std::string	gethost() const;
		void	setservername(const std::string& servername);
		std::string	getservername() const;
		void	setclientbodysize(const std::string& clientbodysize);
		size_t	getclientbodysize() const;
		void	seterrorpage(const std::string& errorpage);
		std::string	geterrorpage() const;
		void		setup(int fd);
	private:
		size_t _port;
		std::string _host, _server_name;
		size_t _client_body_size;
		std::string _error_page;
};

#endif
