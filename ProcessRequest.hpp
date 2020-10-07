/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   ProcessRequest.hpp                                 :+:    :+:            */
/*                                                     +:+                    */
/*   By: sam <sam@student.codam.nl>                   +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/10/06 12:01:32 by sam           #+#    #+#                 */
/*   Updated: 2020/10/06 12:01:32 by sam           ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#ifndef PROCESSREQUEST_HPP
#define PROCESSREQUEST_HPP

#include <vector>
#include <string>

//Accept-Charsets ◦ Accept-Language ◦ Allow
//◦ Authorization
//◦ Content-Language ◦ Content-Length
//◦ Content-Location ◦ Content-Type
//◦ Date ◦ Host

enum headerType {
	ACCEPT_CHARSET,
	ACCEPT_LANGUAGE,
	ALLOW,
	AUTHORIZATION,
	CONTENT_LANGUAGE,
	CONTENT_LENGTH,
	CONTENT_LOCATION,
	CONTENT_TYPE,
	DATE,
	HOST,
	LAST_MODIFIED,
	LOCATION,
	REFERER,
	RETRY_AFTER,
	SERVER,
	TRANSFER_ENCODING,
	USER_AGENT,
	WWW_AUTHENTICATE
};

enum method {
	GET, // These are mandatory according to rfc's
	HEAD, // These are mandatory according to rfc's
	POST,
	PUT
};

class ProcessRequest {
	method _method;
	std::string _uri;
	std::pair<int, int> _version;
	std::vector<std::pair<headerType, std::string> > _headers;
	std::string _rawRequest;
public:
	ProcessRequest();
	virtual ~ProcessRequest();
	void parseRequest();
	void processFirstLine();
};


#endif //PROCESSREQUEST_HPP
