/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Server.hpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: peerdb <peerdb@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/09/29 16:32:46 by peerdb        #+#    #+#                 */
/*   Updated: 2020/09/29 18:41:48 by peerdb        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP
# include <cstddef>
# include <string>

class Server {
	public:
		void		setport(int x) { port = x; }
		int			getport() const { return port; }
		void		setname(std::string x) { server_name = x; }
		std::string	getname() const { return server_name; }
	private:
		size_t		port;
		std::string	server_name;
		
			
};

#endif
