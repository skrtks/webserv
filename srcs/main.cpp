/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   main.cpp                                           :+:    :+:            */
/*                                                     +:+                    */
/*   By: sam <sam@student.codam.nl>                   +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/10/02 16:00:45 by sam           #+#    #+#                 */
/*   Updated: 2020/10/08 22:50:06 by pde-bakk      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

int main(int argc, char **argv) {
	Servermanager manager;
	// std::cout << "Hello world!" << std::endl;
	(void)argc;
	manager = parse(argv[1]);
	for (size_t i = 0; i < manager.size(); i++)
		std::cout << manager[i];
	return 0;
}
