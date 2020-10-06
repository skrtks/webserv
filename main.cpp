/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   main.cpp                                           :+:    :+:            */
/*                                                     +:+                    */
/*   By: sam <sam@student.codam.nl>                   +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/10/02 16:00:45 by sam           #+#    #+#                 */
/*   Updated: 2020/10/02 16:00:45 by sam           ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Connection.hpp"

int main() {
	Connection server;

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
