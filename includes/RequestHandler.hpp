/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   ExecuteHeaders.hpp                                 :+:    :+:            */
/*                                                     +:+                    */
/*   By: skorteka <skorteka@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/10/08 16:15:14 by skorteka      #+#    #+#                 */
/*   Updated: 2020/10/08 16:15:14 by skorteka      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUESTHANDLER_HPP
#define REQUESTHANDLER_HPP

#include "RequestParser.hpp"

class RequestHandler {
	std::map<headerType, void (RequestHandler::*)(const std::string &value)> _functionMap;
	std::string _response;
public:
	RequestHandler();
	virtual ~RequestHandler();
	RequestHandler(const RequestHandler &obj);
	RequestHandler& operator== (const RequestHandler &obj);

	std::string handleRequest(request_s request);

	void handleACCEPT_CHARSET(const std::string &value);
	void handleACCEPT_LANGUAGE(const std::string &value);
	void handleALLOW(const std::string &value);
	void handleAUTHORIZATION(const std::string &value);
	void handleCONTENT_LANGUAGE(const std::string &value);
	void handleCONTENT_LENGTH(const std::string &value);
	void handleCONTENT_LOCATION(const std::string &value);
	void handleCONTENT_TYPE(const std::string &value);
	void handleDATE(const std::string &value);
	void handleHOST(const std::string &value);
	void handleLAST_MODIFIED(const std::string &value);
	void handleLOCATION(const std::string &value);
	void handleREFERER(const std::string &value);
	void handleRETRY_AFTER(const std::string &value);
	void handleSERVER(const std::string &value);
	void handleTRANSFER_ENCODING(const std::string &value);
	void handleUSER_AGENT(const std::string &value);
	void handleWWW_AUTHENTICAT(const std::string &value);
	void generateResponse();
};


#endif //REQUESTHANDLER_HPP
