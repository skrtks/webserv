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

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#define PORT 8080

void setupConnection() {
	int sFd;
	struct sockaddr_in addr = {}; // Will contain info about port and ip

	if ((sFd = socket(PF_INET, SOCK_STREAM, 0) < 0)) {
		throw std::runtime_error("Failed to create socket");
	}

	// Fill struct with info about port and ip
	addr.sin_family = PF_INET; // ipv4
	addr.sin_addr.s_addr = INADDR_ANY; // choose local ip for me
	addr.sin_port = htons(PORT); // set port (htons translates host bit order to network order)

	// Associate the socket with a port and ip
	if ((bind(sFd, (struct sockaddr *)&addr, sizeof(addr)) < 0)) {
		throw std::runtime_error("Failed to bind");
	}

}

int main() {
	try {
		setupConnection();
	}
	catch (std::runtime_error &e) {
		std::cout << e.what() << std::endl;
	}

	return 0;
}
