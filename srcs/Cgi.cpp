/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Cgi.cpp                                            :+:    :+:            */
/*                                                     +:+                    */
/*   By: pde-bakk <pde-bakk@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/10/15 13:54:44 by pde-bakk      #+#    #+#                 */
/*   Updated: 2020/10/15 18:20:35 by pde-bakk      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

// #include "Cgi.hpp"
#include <string>
#include <iostream>
#include <unistd.h>


void	run_cgi(const char *path) {
	extern char	**environ;
	std::cout << "starting run_cgi()" << std::endl;
	
	for (int i = 0; environ[i]; i++) {
		// std::cout << "environ[" << i << "]: " << environ[i] << std::endl;
	}

	pid_t	pid = fork();
	if (pid == 0) {
		// Child process, we run the script here
		std::cout << "child process started!" << std::endl;
		int ret = execve(path, NULL, environ);
		std::cout << "strerror: " << strerror(errno) << std::endl;
		std::cout << "after execve(), ret = " << ret << std::endl;
		exit(0);
	}
	int stat = 0;
	waitpid(-1, &stat, 0);
	std::cout << "parent process returning, stat = " << stat << std::endl;
}

