/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Server.hpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: peerdb <peerdb@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/09/29 16:32:46 by peerdb        #+#    #+#                 */
/*   Updated: 2020/10/15 19:42:06 by pde-bakk      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

#include "Location.hpp"

// enum MetaVariables {
// 	AUTH_TYPE,
// 	CONTENT_LENGTH,
// 	CONTENT_TYPE,
// 	GATEWAY_INTERFACE,
// 	PATH_INFO,
// 	PATH_TRANSLATED,
// 	QUERY_STRING,
// 	REMOTE_ADDR,
// 	REMOTE_IDENT,
// 	REMOTE_USER,
// 	REQUEST_METHOD,
// 	REQUEST_URI,
// 	SCRIPT_NAME,
// 	SERVER_NAME,
// 	SERVER_PORT,
// 	SERVER_PROTOCOL,
// 	SERVER_SOFTWARE
// };

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
		std::map<std::string, std::string> getbaseenv() const;
	
		void		create_base_env();
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
		std::map<std::string, std::string>	_base_env;
};

std::ostream&	operator<<(std::ostream& o, const Server& x);

#endif
