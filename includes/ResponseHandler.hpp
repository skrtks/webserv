/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   ResponseHandler.hpp                                :+:    :+:            */
/*                                                     +:+                    */
/*   By: skorteka <skorteka@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/10/08 16:15:14 by skorteka      #+#    #+#                 */
/*   Updated: 2020/11/28 21:01:34 by tuperera      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSEHANDLER_HPP
#define RESPONSEHANDLER_HPP

#include "RequestParser.hpp"
#include "Cgi.hpp"
//#define SEC_PER_DAY   86400
//#define SEC_PER_HOUR  3600
//#define SEC_PER_MIN   60

class ResponseHandler {
	std::map<headerType, std::string>	_header_vals;
	std::map<headerType, std::string>	_cgi_headers;
	int									_cgi_status_code;
	std::map<int, std::string>			_status_codes;
	std::vector<std::string>			_response;
	std::string							_body;
	int									_status_code;
	Cgi									CGI;
public:
	ResponseHandler();
	virtual ~ResponseHandler();
	ResponseHandler(const ResponseHandler &src);
	ResponseHandler& operator= (const ResponseHandler &rhs);

	std::vector<std::string>	handleRequest(request_s& request);
	void		handleBody(request_s& request);
	void		handleStatusCode(request_s& request);
//	std::string getCurrentDatetime( );
	void		handleALLOW( );
//	void		handleAUTHORIZATION( );  // Not yet implemented
	void		handleCONTENT_LANGUAGE( );
	void		handleCONTENT_LENGTH( );
	void		handleCONTENT_LOCATION( );
	void		handleCONTENT_TYPE(request_s& request);
	void		handleDATE( );
//	void		handleHOST( request_s& request );
//	void		handleLAST_MODIFIED( );
	void		handleLOCATION( std::string& url ); // Not yet used
//	void		handleRETRY_AFTER( ); // Not yet used
	void		handleSERVER( );
	void		handleCONNECTION_HEADER(const request_s& request);
//	void		handleTRANSFER_ENCODING( request_s& request ); // Not yet used
//	void		handleWWW_AUTHENTICATE( ); // Not yet implemented

	void		extractCGIheaders(const std::string&);
	int			authenticate(request_s& request);
	void 		generateResponse(request_s& request);
	int			generatePage(request_s& request);
	void		handlePut(request_s& request);
//	void 		handlePost(request_s& request);
};


#endif //RESPONSEHANDLER_HPP
