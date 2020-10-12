/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   main.cpp                                           :+:    :+:            */
/*                                                     +:+                    */
/*   By: sam <sam@student.codam.nl>                   +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/10/02 16:00:45 by sam           #+#    #+#                 */
/*   Updated: 2020/10/09 15:16:16 by pde-bakk      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

int main(int argc, char **argv) {
	Servermanager manager;
	Connection server;
	// std::cout << "Hello world!" << std::endl;
	(void)argc;
	manager = parse(argv[1]);
	for (size_t i = 0; i < manager.size(); i++)
		std::cout << manager[i];

	server.setServers(manager.getServers());
	try {
		server.setUpConnection();
		server.startListening();
	}
	catch (std::runtime_error &e) {
		std::cout << e.what() << std::endl;
	}
	// TODO: restart server in proper way after exception

	return 0;
}
