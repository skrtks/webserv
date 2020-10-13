/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   main.cpp                                           :+:    :+:            */
/*                                                     +:+                    */
/*   By: peerdb <peerdb@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/10/02 22:22:56 by peerdb        #+#    #+#                 */
/*   Updated: 2020/10/02 22:28:32 by peerdb        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.hpp"

int main() {
	int fd = open("42", O_RDONLY);
	std::string line;
	
	while (get_next_line(fd, line) > 0) {
		std::cout << "line:::: " << line << "::::::" << std::endl;
	}
	exit(0);
}
