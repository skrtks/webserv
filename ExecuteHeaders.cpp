/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   ExecuteHeaders.cpp                                 :+:    :+:            */
/*                                                     +:+                    */
/*   By: skorteka <skorteka@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/10/08 16:15:11 by skorteka      #+#    #+#                 */
/*   Updated: 2020/10/08 16:15:11 by skorteka      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "ExecuteHeaders.hpp"
#include <iostream>

ExecuteHeaders::ExecuteHeaders() {
	_functionMap[ACCEPT_CHARSET] = &ExecuteHeaders::handleACCEPT_CHARSET;
	_functionMap[ACCEPT_LANGUAGE] = &ExecuteHeaders::handleACCEPT_LANGUAGE;
	_functionMap[ALLOW] = &ExecuteHeaders::handleALLOW;
	_functionMap[AUTHORIZATION] = &ExecuteHeaders::handleAUTHORIZATION;
	_functionMap[CONTENT_LANGUAGE] = &ExecuteHeaders::handleCONTENT_LANGUAGE;
	_functionMap[CONTENT_LENGTH] = &ExecuteHeaders::handleCONTENT_LENGTH;
	_functionMap[CONTENT_LOCATION] = &ExecuteHeaders::handleCONTENT_LOCATION;
	_functionMap[CONTENT_TYPE] = &ExecuteHeaders::handleCONTENT_TYPE;
	_functionMap[DATE] = &ExecuteHeaders::handleDATE;
	_functionMap[HOST] = &ExecuteHeaders::handleHOST;
	_functionMap[LAST_MODIFIED] = &ExecuteHeaders::handleLAST_MODIFIED;
	_functionMap[LOCATION] = &ExecuteHeaders::handleLOCATION;
	_functionMap[REFERER] = &ExecuteHeaders::handleREFERER;
	_functionMap[RETRY_AFTER] = &ExecuteHeaders::handleRETRY_AFTER;
	_functionMap[SERVER] = &ExecuteHeaders::handleSERVER;
	_functionMap[TRANSFER_ENCODING] = &ExecuteHeaders::handleTRANSFER_ENCODING;
	_functionMap[USER_AGENT] = &ExecuteHeaders::handleUSER_AGENT;
	_functionMap[WWW_AUTHENTICATE] = &ExecuteHeaders::handleWWW_AUTHENTICAT;
}

ExecuteHeaders::~ExecuteHeaders() {
}

void ExecuteHeaders::startExecution(request_s request) {
	for (std::map<headerType, std::string>::iterator it=request.headers.begin(); it!=request.headers.end(); it++) {
		(this->*(_functionMap.at(it->first)))(it->second);
	}
}

void ExecuteHeaders::handleACCEPT_CHARSET(const std::string &value) {
	std::cout << "Value is: " << value << std::endl;
}

void ExecuteHeaders::handleACCEPT_LANGUAGE(const std::string &value) {
	std::cout << "Value is: " << value << std::endl;
}

void ExecuteHeaders::handleALLOW(const std::string &value) {
	std::cout << "Value is: " << value << std::endl;
}

void ExecuteHeaders::handleAUTHORIZATION(const std::string &value) {
	std::cout << "Value is: " << value << std::endl;
}

void ExecuteHeaders::handleCONTENT_LANGUAGE(const std::string &value) {
	std::cout << "Value is: " << value << std::endl;
}

void ExecuteHeaders::handleCONTENT_LENGTH(const std::string &value) {
	std::cout << "Value is: " << value << std::endl;
}

void ExecuteHeaders::handleCONTENT_LOCATION(const std::string &value) {
	std::cout << "Value is: " << value << std::endl;
}

void ExecuteHeaders::handleCONTENT_TYPE(const std::string &value) {
	std::cout << "Value is: " << value << std::endl;
}

void ExecuteHeaders::handleDATE(const std::string &value) {
	std::cout << "Value is: " << value << std::endl;
}

void ExecuteHeaders::handleHOST(const std::string &value) {
	std::cout << "Value is: " << value << std::endl;
}

void ExecuteHeaders::handleLAST_MODIFIED(const std::string &value) {
	std::cout << "Value is: " << value << std::endl;
}

void ExecuteHeaders::handleLOCATION(const std::string &value) {
	std::cout << "Value is: " << value << std::endl;
}

void ExecuteHeaders::handleREFERER(const std::string &value) {
	std::cout << "Value is: " << value << std::endl;
}

void ExecuteHeaders::handleRETRY_AFTER(const std::string &value) {
	std::cout << "Value is: " << value << std::endl;
}

void ExecuteHeaders::handleSERVER(const std::string &value) {
	std::cout << "Value is: " << value << std::endl;
}

void ExecuteHeaders::handleTRANSFER_ENCODING(const std::string &value) {
	std::cout << "Value is: " << value << std::endl;
}

void ExecuteHeaders::handleUSER_AGENT(const std::string &value) {
	std::cout << "Value is: " << value << std::endl;
}

void ExecuteHeaders::handleWWW_AUTHENTICAT(const std::string &value) {
	std::cout << "Value is: " << value << std::endl;
}
