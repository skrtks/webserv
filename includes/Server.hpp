/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Server.hpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: peerdb <peerdb@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/09/29 16:32:46 by peerdb        #+#    #+#                 */
/*   Updated: 2020/11/10 12:10:49 by peerdb        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

#include "Location.hpp"
#include <cerrno>
#include "Enums.hpp"

class Server {
	public:
		//coplien form
		Server();
		explicit Server(int fd);
		~Server();
		Server(const Server& x);
		Server& 	operator=(const Server& x);

		//setters
		void		setport(const std::string& port);
		void		sethost(const std::string& host);
		void		setindex(const std::string& index);
		void		setroot(const std::string& root);
		void		setservername(const std::string& servername);
		void		setclientbodysize(const std::string& clientbodysize);
		void		seterrorpage(const std::string& errorpage);
		void		set404page(const std::string& page);
		void		setauth_basic_realm(const std::string& realm);
		void		sethtpasswdpath(const std::string& path);
		void		setSocketFd(int socketFd);

		//getters
		size_t					getport() const;
		std::string				gethost() const;
		std::string				getindex() const;
		std::string				getroot() const;
		std::string				getservername() const;
		long int				getclientbodysize() const;
		std::string				geterrorpage() const;
		std::string				get404page() const;
		std::string 			getauthbasicrealm() const;
		std::string				gethtpasswdpath() const;
		Location				matchlocation(const std::string& uri) const;
		int 					getpage(const std::string& uri, std::map<headerType, std::string>&, int&) const;
		std::vector<Location> 	getlocations() const;
		int						getSocketFd() const;
		std::map<std::string, std::string> getbaseenv() const;
	
		void		create_base_env();
		void		configurelocation(const std::string& in);
		void		setup(int fd);
private:
		size_t		_port;
		long int	_client_body_size;
		std::string _host,
					_server_name,
					_error_page,
					_404_page,
					_index,
					_root,
					_auth_basic_realm,
					_htpasswd_path;
		int			_fd, _socketFd;
		std::vector<Location> _locations;
		std::map<std::string, std::string>	_base_env;
};

std::ostream&	operator<<(std::ostream& o, const Server& x);

#endif
