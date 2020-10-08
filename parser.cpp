/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   parser.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: peerdb <peerdb@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/09/29 16:36:33 by peerdb        #+#    #+#                 */
/*   Updated: 2020/10/08 16:52:52 by pde-bakk      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

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
	size_t vend = str.find_first_of("\n\r#;", vbegin);
	value = str.substr(vbegin, vend - vbegin);
}

void	parse(char *av) {
	struct stat statstruct;
	int fd;
	if (av && stat(av, &statstruct) != -1)
		fd = open(av, O_RDONLY);
	else
		fd = open("configfiles/nginx.conf", O_RDONLY);
	if (fd < 0)
		return ;
	std::string	str;
	Servermanager	skrtks;
	Server	tmp;
	
	while (get_next_line(fd, str) > 0) {
		if (is_first_char(str))
			continue ;
		try {
			if (str != "" && str.compare(str.find_first_not_of(" \t\n"), ft_strlen("server {"), "server {") == 0) {
				tmp.setup(fd);
				if (tmp.getsuccess()) {
					skrtks += tmp;
					// std::cout << "success yes" << std::endl;
				}
				tmp.clear();
			}
		}
		catch (std::exception& e) {
			std::cerr << "haha" << std::endl;
		}
	}
	close(fd);
	for (size_t i = 0; i < skrtks.size(); i++)
		std::cout << skrtks[i];
}
