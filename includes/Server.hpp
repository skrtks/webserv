/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Server.hpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: peerdb <peerdb@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/09/29 16:32:46 by peerdb        #+#    #+#                 */
/*   Updated: 2020/11/23 17:18:05 by pde-bakk      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include "Base64.hpp"
# include "Location.hpp"
# include "Enums.hpp"
# include "Colours.hpp"
# include <cerrno>
# include <fcntl.h>
# include <iostream>
# include <zconf.h>
# include <sys/stat.h>

class Server {
	public:
		//coplien form
		Server();
		explicit Server(int fd);
		~Server();
		Server(const Server& x);
		Server& 	operator=(const Server& x);

private:	//setters
		void		setport(const std::string& port);
		void		sethost(const std::string& host);
		void		setindexes(const std::string& index);
		void		setroot(const std::string& root);
		void		setservername(const std::string& servername);
		void		setclientbodysize(const std::string& clientbodysize);
		void		seterrorpage(const std::string& errorpage);
		void		set404page(const std::string& page);
		void		setauth_basic_realm(const std::string& realm);
		void		sethtpasswdpath(const std::string& path);
		void		setautoindex(const std::string& ai);
		void		configurelocation(const std::string& in);
public:
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
		int 					getpage(const std::string& uri, std::map<headerType, std::string>&, int&) const;
		std::vector<Location> 	getlocations() const;
		int						getSocketFd() const;
		std::string 			getautoindex() const;

		Location	matchlocation(const std::string& uri) const;
		std::string	getfilepath(const std::string& uri) const;
		void		setup(int fd);
		bool		getmatch(const std::string& username, const std::string& passwd);
		bool		isMethodAllowed(const std::string& uri, const std::string& extension) const;
private:
		size_t		_port;
		long int	_client_body_size;
		std::string _host,
					_server_name,
					_error_page,
					_404_page,
					_root,
					_auth_basic_realm,
					_htpasswd_path,
					_autoindex;
		int			_fd, _socketFd;
		std::vector<std::string> _indexes;
		std::vector<Location> _locations;
		std::map<std::string, std::string>	_loginfo;
};

std::ostream&	operator<<(std::ostream& o, const Server& x);

#endif
