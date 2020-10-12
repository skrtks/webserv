/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Servermanager.hpp                                  :+:    :+:            */
/*                                                     +:+                    */
/*   By: pde-bakk <pde-bakk@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/10/07 20:23:33 by pde-bakk      #+#    #+#                 */
/*   Updated: 2020/10/09 15:27:16 by pde-bakk      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERMANAGER_HPP
# define SERVERMANAGER_HPP

# include <cstddef>
# include <vector>

class Server;

class Servermanager {
	public:
		Servermanager();
		~Servermanager();
		Servermanager(const Servermanager& x);
		Servermanager&	operator=(const Servermanager& x);
		Servermanager&	operator +=(const Server& x);
		Server		operator[](const size_t n) const;
		size_t		size() const;
		void		clear();
	const std::vector<Server>& getServers() const;
private:
		std::vector<Server> _servers;
};

#endif
