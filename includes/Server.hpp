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

# include <vector>
# include	"Location.hpp"
# include	"Client.hpp"

#define BACKLOG 128

class Server {
	public:
		//coplien form
		Server();
		explicit Server(int fd);
		~Server();
		Server(const Server& x);
		Server& 	operator=(const Server& x);
		friend class Connection;

private:	//setters
		void		setport(const std::string& );
		void		sethost(const std::string& );
		void		setindexes(const std::string& );
		void		setroot(const std::string& );
		void		setservername(const std::string& );
		void		setmaxfilesize(const std::string& );
		void		seterrorpage(const std::string& );
		void		setauth_basic_realm(const std::string& );
		void		sethtpasswdpath(const std::string& );
		void		setautoindex(const std::string& );
		void		configurelocation(const std::string& );
public:
		void		startListening();
		int			addConnection();
		void		showclients(const fd_set& readfds, const fd_set& writefds);
		std::vector<Client*> _connections;

		//getters
		size_t					getport() const;
		std::string				gethost() const;
		std::string				getindex() const;
		std::string				getroot() const;
		std::string				getservername() const;
		long int				getmaxfilesize() const;
		std::string				geterrorpage() const;
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
		bool		isExtensionAllowed(const std::string& uri) const;
private:
		size_t		_port;
		long int	_maxfilesize;
		std::string _host,
					_server_name,
					_error_page,
					_root,
					_auth_basic_realm,
					_htpasswd_path,
					_autoindex;
		int			_fd,
					_socketFd;
		struct sockaddr_in	addr;
		std::vector<std::string> _indexes;
		std::vector<Location> _locations;
		std::map<std::string, std::string>	_loginfo;
};

std::ostream&	operator<<(std::ostream& o, const Server& x);

#endif
