/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   parser.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: peerdb <peerdb@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/09/29 16:36:33 by peerdb        #+#    #+#                 */
/*   Updated: 2020/10/02 19:56:00 by peerdb        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include <string>
#include <iostream>
#include <fcntl.h>
#include <unistd.h>
extern "C" {
	#include "libft/libft.h"
	#include "getnextline/get_next_line.h"
}
#define COMMENT_CHAR '#'

int		is_comment(std::string str) {
	int i = 0;
	while (str[i] && iswhitespace(str[i]))
		++i;
	if (str[i] == COMMENT_CHAR)
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

void	new_server(int fd) {
	char *line;
	Server serv;
	std::string str;

	while (get_next_line(fd, &line) > 0) {
		std::string key, value;
		str = line;
		free(line);
		if (is_comment(str))
			continue ;
		try {
			get_key_value(str, key, value);
			std::cout << "key = " << key << ", & value = " << value << std::endl;
		}
		catch (std::exception& e) { }
	}
}

void	parse() {
	char	*line;
	// int		brackets = 0;
	int		fd = open("default.conf", O_RDONLY);
	std::string	str;
	Server	serv;
	
	while (get_next_line(fd, &line) > 0) {
		str = line;
		free(line);
		if (is_comment(str)) {
			// std::cout << str << " is a comment" << std::endl;
			continue ;
		}
		try {
			if (str != "" && str.compare(str.find_first_not_of(" \t\n"), ft_strlen("server {"), "server {") == 0)
				new_server(fd);
		}
		catch (std::exception& e) {
			std::cerr << "haha" << std::endl;
		}
		// std::cout << "after " << line << ", brackets = " << brackets << std::endl;
		
	}
	
}


int main() {
	parse();
}
