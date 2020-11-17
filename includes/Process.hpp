/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   HandleCli.hpp                                      :+:    :+:            */
/*                                                     +:+                    */
/*   By: sam <sam@student.codam.nl>                   +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/11/17 11:08:08 by sam           #+#    #+#                 */
/*   Updated: 2020/11/17 11:08:08 by sam           ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#ifndef PROCESS_HPP
#define PROCESS_HPP

#include <string>
#include "Servermanager.hpp"
#include "Connection.hpp"
#include "libftGnl.hpp"

class Process {
	Servermanager _manager;
	Connection _server;
	char *_configPath;
public:
	Process();
	explicit Process(char* configPath);
	virtual ~Process();
	Process(const Process& obj);
	Process& operator=(const Process& obj);

	void startServer();
	void loadConfiguration();
	void handleCLI(std::string input);
};


#endif //PROCESS_HPP
