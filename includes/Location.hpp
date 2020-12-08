/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Location.hpp                                       :+:    :+:            */
/*                                                     +:+                    */
/*   By: pde-bakk <pde-bakk@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/10/08 14:26:27 by pde-bakk      #+#    #+#                 */
/*   Updated: 2020/10/15 18:26:22 by pde-bakk      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATION_HPP
# define LOCATION_HPP

# include <string>
# include <map>
# include <vector>
# include "Enums.hpp"

class Location {
public:
	friend class Server;
	//coplien form
	Location();
	explicit Location(std::string& );
	~Location();
	Location(const Location& );
	Location&	operator=(const Location& );

private:		//setters
		void	setroot(const std::string& );
		void	setautoindex(const std::string& );
		void	setallow_method(const std::string& );
		void	setindex(const std::string& );
		void	setcgiallowedextensions(const std::string& );
		void	seterrorpage(const std::string& );
		void	setmaxbody(const std::string& );
		
public:			//getters
		std::string					getroot() const;
		std::string					getautoindex() const;
		std::string					getlocationmatch() const;
		std::vector<e_method>		getallowmethods() const;
		std::string					getindex() const;
		std::vector<std::string>	getindexes() const;
		std::vector<std::string>	getcgiallowedextensions() const;
		std::string					geterrorpage() const;
		long						getmaxbody() const;
		
		void	setup(int );
		bool	checkifMethodAllowed(const e_method& meth) const;
		void	addServerInfo(const std::string& root, const std::string& autoindex,
					 const std::vector<std::string>& indexes, const std::string& errorpage);

private:
	std::string					_root,
								_autoindex,
								_location_match,
								_error_page;
	std::vector<std::string>	_indexes,
								_cgi_allowed_extensions;
	std::vector<e_method>		_allow_method;
	long						_maxBody;
};

std::ostream&	operator<<(std::ostream& o, const Location& x);

#endif
