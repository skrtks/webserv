//
// Created by peerdb on 02-12-20.
//

#ifndef WEBSERV_CGI_HPP
#define WEBSERV_CGI_HPP
#include <map>
#include <string>
#include "Colours.hpp"
#include "Server.hpp"
#include "RequestParser.hpp"
#include "libftGnl.hpp"

class Cgi {
public:
	Cgi();
	Cgi(const Cgi&);
	Cgi&	operator=(const Cgi&);
	~Cgi();

	int		run_cgi(request_s&);

private:
	void	populate_map(request_s&);
	void	map_to_env();
	void	clear_env();

	std::map<std::string, std::string>	_m;
	char**								_env;
};

#endif //WEBSERV_CGI_HPP
