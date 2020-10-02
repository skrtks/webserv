/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   parser.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: peerdb <peerdb@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/09/29 16:36:33 by peerdb        #+#    #+#                 */
/*   Updated: 2020/10/02 17:17:25 by peerdb        ########   odam.nl         */
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

	while (iswhitespace(str[i]))
		++i;
	if (str[i] == COMMENT_CHAR)
		return 1;
	return 0;
}

void	new_server(int fd) {
	(void)fd;
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
			std::cout << str << " is a comment" << std::endl;
			continue ;
		}
		if (str.compare(0, ft_strlen("server {"), "server {") == 0)
			new_server(fd);
		// std::cout << "after " << line << ", brackets = " << brackets << std::endl;
		
	}
	
}


int main() {
	parse();
}
