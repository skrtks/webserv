/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   main.cpp                                           :+:    :+:            */
/*                                                     +:+                    */
/*   By: sam <sam@student.codam.nl>                   +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/10/02 16:00:45 by sam           #+#    #+#                 */
/*   Updated: 2020/10/08 14:44:28 by pde-bakk      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

int main(int argc, char **argv) {
	// std::cout << "Hello world!" << std::endl;
	(void)argc;
	parse(argv[1]);
	return 0;
}
