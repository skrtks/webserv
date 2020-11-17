/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   main.cpp                                           :+:    :+:            */
/*                                                     +:+                    */
/*   By: sam <sam@student.codam.nl>                   +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/10/02 16:00:45 by sam           #+#    #+#                 */
/*   Updated: 2020/10/13 16:24:06 by pde-bakk      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Process.hpp"

int main(int argc, char **argv) {
	Process process(argv[1]);
	(void)argc;

	try {
		process.startServer();
	}
	catch (std::runtime_error &e) {
		std::cout << e.what() << std::endl;
	}

	return 0;
}
