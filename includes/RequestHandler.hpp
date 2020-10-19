/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   RequestHandler.hpp                                 :+:    :+:            */
/*                                                     +:+                    */
/*   By: skorteka <skorteka@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/10/08 16:15:14 by skorteka      #+#    #+#                 */
/*   Updated: 2020/10/20 00:08:28 by peerdb        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUESTHANDLER_HPP
#define REQUESTHANDLER_HPP

#include "RequestParser.hpp"

#define SEC_PER_DAY   86400
#define SEC_PER_HOUR  3600
#define SEC_PER_MIN   60

class RequestHandler {
	std::map<headerType, void (RequestHandler::*)( void )>	_functionMap;
	std::map<headerType, std::string>						_header_vals;
	std::string												_response;
	std::string												_body;
	int														_body_length;
public:
	RequestHandler();
	virtual ~RequestHandler();
	RequestHandler(const RequestHandler &src);
	RequestHandler& operator= (const RequestHandler &rhs);

	std::string	handleRequest(request_s request);

	void		handleACCEPT_CHARSET( void );
	void		handleACCEPT_LANGUAGE( void );
	void		handleALLOW( void );
	void		handleAUTHORIZATION( void );
	void		handleCONTENT_LANGUAGE( void );
	void		handleCONTENT_LENGTH( void );
	void		handleCONTENT_LOCATION( void );
	void		handleCONTENT_TYPE( void );
	void		handleDATE( void );
	void		handleHOST( void );
	void		handleLAST_MODIFIED( void );
	void		handleLOCATION( void );
	void		handleREFERER( void );
	void		handleRETRY_AFTER( void );
	void		handleSERVER( void );
	void		handleTRANSFER_ENCODING( void );
	void		handleUSER_AGENT( void );
	void		handleWWW_AUTHENTICATE( void );
	int			run_cgi(const request_s& request);
	void		generateResponse(request_s& request);
};


#endif //REQUESTHANDLER_HPP
