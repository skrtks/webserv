/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   HandleCli.cpp                                      :+:    :+:            */
/*                                                     +:+                    */
/*   By: sam <sam@student.codam.nl>                   +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/11/17 11:08:10 by sam           #+#    #+#                 */
/*   Updated: 2020/11/17 11:08:10 by sam           ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include <list>
#include "Process.hpp"

Process::Process(char *configPath) {
	_configPath = configPath;
	loadConfiguration();
}

Process::Process() {
	_configPath = NULL;
	loadConfiguration();
}

Process::~Process() {
}

Process::Process(const Process& obj) : _configPath(obj._configPath) {
	*this = obj;
}

Process& Process::operator=(const Process& obj) {
	if (this != &obj) {
		*this = obj;
	}
	return (*this);
}

void Process::startServer() {
	loadConfiguration();
	_server.setUpConnection();
	_server.startListening();
}

void Process::loadConfiguration() {
	_manager = parse(_configPath);
	for (size_t i = 0; i < _manager.size(); i++)
		std::cout << _manager[i];

	_server.setServers(_manager.getServers());
}

void Process::handleCLI(std::string input) {
	if (input == "exit") {
		std::cout << "Shutting down..." << std::endl;
		exit(0);
	}
	else if (input == "restart") {
		std::cout << "Restarting server..." << std::endl;
//		stopServer();
		startServer();
	}
	else if (input == "help") {
		std::cout << "These are common Git commands used in various situations:\n"
					 "\n"
					 "start a working area (see also: git help tutorial)\n"
					 "   clone             Clone a repository into a new directory\n"
					 "   init              Create an empty Git repository or reinitialize an existing one\n"
					 "\n"
					 "work on the current change (see also: git help everyday)\n"
					 "   add               Add file contents to the index\n"
					 "   mv                Move or rename a file, a directory, or a symlink\n"
					 "   restore           Restore working tree files\n"
					 "   rm                Remove files from the working tree and from the index\n"
					 "   sparse-checkout   Initialize and modify the sparse-checkout" << std::endl;
	}
	else {
		std::cout << "Command \"" << input << "\" not found. Type \"help\" for available commands" << std::endl;
	}
}
