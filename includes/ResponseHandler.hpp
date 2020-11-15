/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   ResponseHandler.hpp                                 :+:    :+:            */
/*                                                     +:+                    */
/*   By: skorteka <skorteka@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/10/08 16:15:14 by skorteka      #+#    #+#                 */
/*   Updated: 2020/10/20 00:08:28 by peerdb        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSEHANDLER_HPP
#define RESPONSEHANDLER_HPP

#include "RequestParser.hpp"

#define SEC_PER_DAY   86400
#define SEC_PER_HOUR  3600
#define SEC_PER_MIN   60

class ResponseHandler {
	std::map<headerType, std::string>	_header_vals;
	std::map<int, std::string>			_status_codes;
	std::string							_response;
	std::string							_body;
	int									_body_length;
	int									_status_code;
public:
	ResponseHandler();
	virtual ~ResponseHandler();
	ResponseHandler(const ResponseHandler &src);
	ResponseHandler& operator= (const ResponseHandler &rhs);

	std::string	handleRequest(request_s request);
	void		handleBody(request_s& request);
	void		handleStatusCode(request_s& request);
	std::string getCurrentDatetime(void );
	void		handleALLOW( void );
	void		handleAUTHORIZATION( void );  // Not yet implemented
	void		handleCONTENT_LANGUAGE( void );
	void		handleCONTENT_LENGTH( void );
	void		handleCONTENT_LOCATION( void );
	void		handleCONTENT_TYPE(request_s& request);
	void		handleDATE( void );
	void		handleHOST( request_s& request );
	void		handleLAST_MODIFIED( void );
	void		handleLOCATION( void ); // Not yet used
	void		handleRETRY_AFTER( void ); // Not yet used
	void		handleSERVER( void );
	void		handleTRANSFER_ENCODING( request_s& request ); // Not yet used
	void		handleWWW_AUTHENTICATE( void ); // Not yet implemented

	int			authenticate(request_s& request);
	void 		generateResponse(request_s& request);
	int			run_cgi(const request_s& request);
	int			generatePage(request_s& request);
};


#endif //RESPONSEHANDLER_HPP
