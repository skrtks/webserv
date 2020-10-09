/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   get_next_line.cpp                                  :+:    :+:            */
/*                                                     +:+                    */
/*   By: peerdb <peerdb@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/10/02 21:58:28 by peerdb        #+#    #+#                 */
/*   Updated: 2020/10/09 14:46:50 by pde-bakk      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.hpp"

namespace ft {

	static int	finish(std::string& buf, std::string& str, std::string& line, int ret)
	{
		buf = str.substr(str.find_first_of('\n') + 1, str.length() - str.find_first_of('\n') - 1);
		line = str.substr(0, str.find_first_of('\n'));
		return (ret > 0);
	}

	int	get_next_line(int fd, std::string& line)
	{
		static std::string	buf;
		std::string			str;
		int					ret = 1;

		while (ret > 0) {
			str += buf;
			if (str.find("\n\0") != str.npos) {
				return (finish(buf, str, line, ret));
			}
			buf.clear();
			char *tmp = (char*) malloc(BUFFER_SIZE + 1);
			for (int i = 0; i < BUFFER_SIZE + 1; i++)
				tmp[i] = 0;
			ret = read(fd, tmp, BUFFER_SIZE);
			buf.assign(tmp);
			free(tmp);
		}
		return 0;
	}

}
