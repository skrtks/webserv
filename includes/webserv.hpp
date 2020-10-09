/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   webserv.hpp                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: pde-bakk <pde-bakk@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/10/07 15:37:16 by pde-bakk      #+#    #+#                 */
/*   Updated: 2020/10/09 15:19:45 by pde-bakk      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include <cstddef>
# include <cstring>
# include <string>
# include <map>
# include <vector>
# include <list>
# include <stack>
# include <queue>
# include <iostream>
# include <fcntl.h>
# include <unistd.h>
# include <sys/stat.h>

# include "../getnextline/get_next_line.hpp"
# include "Server.hpp"
# include "Connection.hpp"
# include "Servermanager.hpp"
# include "Location.hpp"
extern "C" {
	# include "../libft/libft.h"
}

class Servermanager;
// parser
int							is_first_char(std::string str, char find = '#');
void						get_key_value(std::string &str, std::string &key, std::string& value);
Servermanager				parse(char *av);

namespace ft {
	std::vector<std::string>	split(const std::string& s, const std::string& delim);	
}


#endif
