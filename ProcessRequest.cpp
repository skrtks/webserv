/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   ProcessRequest.cpp                                 :+:    :+:            */
/*                                                     +:+                    */
/*   By: sam <sam@student.codam.nl>                   +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/10/06 12:01:35 by sam           #+#    #+#                 */
/*   Updated: 2020/10/06 12:01:35 by sam           ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "ProcessRequest.hpp"

ProcessRequest::ProcessRequest() {
	_rawRequest = "GET /yosemite.jpg HTTP/1.1\n"
				  "Host: localhost:8080\n"
				  "User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10.14; rv:81.0) Gecko/20100101 Firefox/81.0\n"
				  "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8\n"
				  "Accept-Language: nl,en-US;q=0.7,en;q=0.3\n"
				  "Accept-Encoding: chunked\n"
				  "DNT: 1\n"
				  "Connection: keep-alive\n"
				  "Upgrade-Insecure-Requests: 1\n"
				  "Pragma: no-cache\n"
				  "Cache-Control: no-cache\n";
}

ProcessRequest::~ProcessRequest() {
}

void ProcessRequest::parseRequest() {

}
