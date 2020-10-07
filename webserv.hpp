/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   webserv.hpp                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: pde-bakk <pde-bakk@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/10/07 15:37:16 by pde-bakk      #+#    #+#                 */
/*   Updated: 2020/10/07 15:44:53 by pde-bakk      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include <cstddef>
# include <string>
# include "getnextline/get_next_line.hpp"
# include <map>
# include <vector>
# include <list>
# include <stack>
# include <queue>
# include <iostream>
# include <fcntl.h>
# include <unistd.h>
extern "C" {
	#include "libft/libft.h"
}

// parser
int		is_first_char(std::string str, char find = '#');
void	get_key_value(std::string &str, std::string &key, std::string& value);
void	parse();

#endif
