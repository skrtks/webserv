/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   RequestParser.hpp                                  :+:    :+:            */
/*                                                     +:+                    */
/*   By: sam <sam@student.codam.nl>                   +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/10/06 12:01:32 by sam           #+#    #+#                 */
/*   Updated: 2020/11/28 19:45:53 by tuperera      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUESTPARSER_HPP
#define REQUESTPARSER_HPP

#include <map>
#include <stdexcept>
#include <string>
#include <cstring>
#include <sys/time.h>
#include <ctime>
#include <sstream>
#include "Server.hpp"

enum e_method {
	GET, // These are mandatory according to rfc's
	HEAD, // These are mandatory according to rfc's
	POST,
	PUT
};

struct request_s {
	int									status_code;
	e_method							method;
	std::string							uri;
	std::pair<int, int>					version;
	std::map<headerType, std::string>	headers;
	Server								server;
	std::string							body;
	bool								transfer_buffer;
};

class RequestParser {
	int									_status_code;
	e_method							_method;
	std::string							_uri;
	std::pair<int, int>					_version;
	std::map<headerType, std::string>	_headers;
	std::map<std::string, e_method>		_methodMap;
	std::map<std::string, headerType>	_headerMap;
	std::string							_rawRequest;

public:
	RequestParser();
	virtual ~RequestParser();
	RequestParser(const RequestParser &obj);
	RequestParser&	operator= (const RequestParser &obj);

	request_s		parseRequest(const std::string &req);
	void			parseRequestLine();
	void			parseHeaders();
	void			setRawRequest(const std::string& rawRequest);
	void			extractMethod(size_t eoRequestLine, size_t& pos);
	void			extractUri(size_t eoRequestLine, size_t pos, size_t pos2);
	void			extractVersion(size_t eoRequestLine, size_t& pos, size_t &pos2);
	e_method		getMethod() const;
	static std::string		getMethod(e_method& x) ;
	
	const std::string&							getUri() const;
	const std::pair<int, int>& 					getVersion() const;
	const std::map<headerType, std::string>&	getHeaders() const;
};


#endif //REQUESTPARSER_HPP
