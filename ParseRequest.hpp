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

#ifndef PARSEREQUEST_HPP
#define PARSEREQUEST_HPP

#include <vector>
#include <string>
#include <string.h>
#include <map>

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

struct request_s {
	method method;
	std::string uri;
	std::pair<int, int> version;
	std::map<headerType, std::string> headers;
};

class ParseRequest {
	method _method;
	std::string _uri;
	std::pair<int, int> _version;
	std::map<headerType, std::string> _headers;
	std::map<std::string, method> _methodMap;
	std::map<std::string, headerType> _headerMap;
	std::string _rawRequest;
public:
	ParseRequest();
	virtual ~ParseRequest();
	request_s parseRequest(const std::string &req);
	void parseRequestLine();
	method getMethod() const;
	const std::map<headerType, std::string>& getHeaders() const;
	const std::string& getUri() const;
	const std::pair<int, int>& getVersion() const;
	void setRawRequest(const std::string& rawRequest);
	void extractMethod(size_t eoRequestLine, size_t& pos);
	void extractUri(size_t eoRequestLine, size_t pos, size_t pos2);
	void extractVersion(size_t eoRequestLine, size_t& pos, size_t &pos2);
	void parseHeaders();
};


#endif //PARSEREQUEST_HPP
