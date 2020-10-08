/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Location.hpp                                       :+:    :+:            */
/*                                                     +:+                    */
/*   By: pde-bakk <pde-bakk@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/10/08 14:26:27 by pde-bakk      #+#    #+#                 */
/*   Updated: 2020/10/08 15:01:53 by pde-bakk      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATION_HPP
# define LOCATION_HPP

# include "webserv.hpp"

class Location {
	public:
		Location();
		~Location();
		Location(const Location& x);
		Location&	operator=(const Location& x);

		void	setroot(const std::string& );
		void	setautoindex(const std::string& );
		void	setallow_method(const std::string& );
		void	setindex(const std::string& );
		void	setcgi(const std::string& );
		
		void	setup(int fd);

	private:
	std::string					_root,
								_autoindex;
	std::vector<std::string>	_allow_method,
								_indexes,
								_cgi;
};

#endif
