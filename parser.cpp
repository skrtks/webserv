/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   parser.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: peerdb <peerdb@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/09/29 16:36:33 by peerdb        #+#    #+#                 */
/*   Updated: 2020/10/07 19:14:26 by pde-bakk      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include <string>
#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <map>
#include "getnextline/get_next_line.hpp"
extern "C" {
	#include "libft/libft.h"
}

int		is_first_char(std::string str, char find) {
	int i = 0;
	while (str[i] && iswhitespace(str[i]))
		++i;
	if (str[i] == find)
		return (1);
	return (0);
}

void	get_key_value(std::string &str, std::string &key, std::string& value) {
	size_t kbegin = str.find_first_not_of(" \t\n");
	size_t kend = str.find_first_of(" \t\n", kbegin);
	key = str.substr(kbegin, kend - kbegin);
	size_t vbegin = str.find_first_not_of(" \t\n", kend);
	size_t vend = str.find_first_of(" \t\n\r", vbegin);
	value = str.substr(vbegin, vend - vbegin);
}

Server	new_server(int fd) {
	Server serv;
	serv.setup(fd);
	return serv;
}

void	parse() {
	// int		brackets = 0;
	int		fd = open("nginx.conf", O_RDONLY);
	std::string	str;
	Server	tmp;
	
	while (get_next_line(fd, str) > 0) {
		if (is_first_char(str))
			continue ;
		try {
			if (str != "" && str.compare(str.find_first_not_of(" \t\n"), ft_strlen("server {"), "server {") == 0)
				tmp = new_server(fd);
		}
		catch (std::exception& e) {
			std::cerr << "haha" << std::endl;
		}
	}
}
