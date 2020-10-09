#define CATCH_CONFIG_MAIN
#include "Catch2.h"
#include "ParseRequest.hpp"

TEST_CASE( "Parsing request", "[Request]" ) {

	ParseRequest reqProcces;

	SECTION( "Correct input 1" ) {
		std::string request = "GET /index.html HTTP/1.1\r\nData: bla\r\n";
		reqProcces.setRawRequest(request);

		reqProcces.parseRequestLine();

		REQUIRE(reqProcces.getMethod() == GET);
		REQUIRE(reqProcces.getUri() == "/index.html");
		std::pair<int, int> version(1, 1);
		REQUIRE(reqProcces.getVersion() == version);
	}
	SECTION( "Correct input 2" ) {
		std::string request = "POST /index HTTP/0.9\r\n";
		reqProcces.setRawRequest(request);

		reqProcces.parseRequestLine();

		REQUIRE(reqProcces.getMethod() == POST);
		REQUIRE(reqProcces.getUri() == "/index");
		std::pair<int, int> version(0, 9);
		REQUIRE(reqProcces.getVersion() == version);
	}
	SECTION( "Correct input 3" ) {
		std::string request = "HEAD / HTTP/1.0\r\n";
		reqProcces.setRawRequest(request);

		reqProcces.parseRequestLine();

		REQUIRE(reqProcces.getMethod() == HEAD);
		REQUIRE(reqProcces.getUri() == "/");
		std::pair<int, int> version(1, 0);
		REQUIRE(reqProcces.getVersion() == version);
	}
	SECTION( "incorrect input 1" ) {
		std::string request = "HEA / HTTP/1.0\r\n";
		reqProcces.setRawRequest(request);

		REQUIRE_THROWS_WITH(reqProcces.parseRequestLine(), "Incorrect value for method" );
	}
	SECTION( "incorrect input 2" ) {
		std::string request = "HEAD/ HTTP/1.0\r\n";
		reqProcces.setRawRequest(request);

		REQUIRE_THROWS(reqProcces.parseRequestLine() );
	}
	SECTION( "incorrect input 3" ) {
		std::string request = "HEAD /HTTP/1.0\r\n";
		reqProcces.setRawRequest(request);

		REQUIRE_THROWS(reqProcces.parseRequestLine() );
	}
	SECTION( "incorrect input 4" ) {
		std::string request = "HEAD / HTTP /1.0\r\n";
		reqProcces.setRawRequest(request);

		REQUIRE_THROWS(reqProcces.parseRequestLine() );
	}
	SECTION( "incorrect input 5" ) {
		std::string request = "HEAD / HTTP/1. 0\r\n";
		reqProcces.setRawRequest(request);

		REQUIRE_THROWS(reqProcces.parseRequestLine() );
	}
	SECTION( "incorrect input 6" ) {
		std::string request = "HEAD / HTTP/1.0 \r\n";
		reqProcces.setRawRequest(request);

		REQUIRE_THROWS(reqProcces.parseRequestLine() );
	}
	SECTION( "incorrect input 7" ) {
		std::string request = "HEAD/  HTTP/1.0\r\n";
		reqProcces.setRawRequest(request);

		REQUIRE_THROWS(reqProcces.parseRequestLine() );
	}
	SECTION( "incorrect input 8" ) {
		std::string request = "  HEAD/HTTP/1.0\r\n";
		reqProcces.setRawRequest(request);

		REQUIRE_THROWS(reqProcces.parseRequestLine() );
	}
	SECTION( "incorrect input 9" ) {
		std::string request = "HEAD/HTTP/1.0  \r\n";
		reqProcces.setRawRequest(request);

		REQUIRE_THROWS(reqProcces.parseRequestLine() );
	}
	SECTION( "incorrect input 10" ) {
		std::string request = "HEAD/HT TP/1. 0\r\n";
		reqProcces.setRawRequest(request);

		REQUIRE_THROWS(reqProcces.parseRequestLine() );
	}
	SECTION( "incorrect input 11" ) {
		std::string request = "HEAD /HTTP /1.0\r\n";
		reqProcces.setRawRequest(request);

		REQUIRE_THROWS(reqProcces.parseRequestLine() );
	}
	SECTION( "incorrect input 12" ) {
		std::string request = "HEAD/ HTTP/ 1.0\r\n";
		reqProcces.setRawRequest(request);

		REQUIRE_THROWS(reqProcces.parseRequestLine() );
	}
	SECTION( "incorrect input 13" ) {
		std::string request = "HEAD/ HTTP/1.0 \r\n";
		reqProcces.setRawRequest(request);

		REQUIRE_THROWS(reqProcces.parseRequestLine() );
	}
	SECTION( "incorrect input 14" ) {
		std::string request = "HEAD / HTTP/1.0 \r\n";
		reqProcces.setRawRequest(request);

		REQUIRE_THROWS(reqProcces.parseRequestLine() );
	}
	SECTION( "incorrect input 15" ) {
		std::string request = "HEAD / HTTP/1.0 \n";
		reqProcces.setRawRequest(request);

		REQUIRE_THROWS(reqProcces.parseRequestLine() );
	}
	SECTION( "incorrect input 16" ) {
		std::string request = "HEAD / HTTP/1.0\n";
		reqProcces.setRawRequest(request);

		REQUIRE_THROWS(reqProcces.parseRequestLine() );
	}
	SECTION( "incorrect input 16" ) {
		std::string request = "HEAD / HTTP/1.0";
		reqProcces.setRawRequest(request);

		REQUIRE_THROWS(reqProcces.parseRequestLine() );
	}
}

TEST_CASE( "Parse Headers", "[Request]") {
	ParseRequest reqProcces;

	SECTION( "correct input 1" ) {
		std::string request = "Host: localhost:8080\r\n"
							  "User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10.14; rv:68.0) Gecko/20100101 Firefox/68.0\r\n"
							  "location: Amsterdam\r\n"
							  "Accept-Language: en-US,en;q=0.5\r\n";

		reqProcces.setRawRequest(request);

		reqProcces.parseHeaders();
		std::map<headerType, std::string> headers = reqProcces.getHeaders();
		for (std::map<headerType, std::string>::iterator it=headers.begin(); it!=headers.end(); it++)
			switch (it->first) {
				case ACCEPT_LANGUAGE:
					REQUIRE(it->second == "en-US,en;q=0.5");
					break;
				case HOST:
					REQUIRE(it->second == "localhost:8080");
					break;
				case LOCATION:
					REQUIRE(it->second == "Amsterdam");
					break;
				case USER_AGENT:
					REQUIRE(it->second == "Mozilla/5.0 (Macintosh; Intel Mac OS X 10.14; rv:68.0) Gecko/20100101 Firefox/68.0");
					break;
				default:
					REQUIRE(headers.empty());
			}
	}

	SECTION( "correct input 2" ) {
		std::string request = "Host:    localhost:8080\r\n"
							  "User-Agent:    Mozilla/5.0 (Macintosh; Intel Mac OS X 10.14; rv:68.0) Gecko/20100101 Firefox/68.0\r\n"
							  "location:   Amsterdam\r\n"
							  "Accept-Language:en-US,en;q=0.5\r\n";

		reqProcces.setRawRequest(request);

		reqProcces.parseHeaders();
		std::map<headerType, std::string> headers = reqProcces.getHeaders();
		for (std::map<headerType, std::string>::iterator it=headers.begin(); it!=headers.end(); it++)
			switch (it->first) {
				case ACCEPT_LANGUAGE:
					REQUIRE(it->second == "en-US,en;q=0.5");
					break;
				case HOST:
					REQUIRE(it->second == "localhost:8080");
					break;
				case LOCATION:
					REQUIRE(it->second == "Amsterdam");
					break;
				case USER_AGENT:
					REQUIRE(it->second == "Mozilla/5.0 (Macintosh; Intel Mac OS X 10.14; rv:68.0) Gecko/20100101 Firefox/68.0");
					break;
				default:
					REQUIRE(headers.empty());
			}
	}

	SECTION( "correct input 3" ) {
		std::string request = "";

		reqProcces.setRawRequest(request);

		reqProcces.parseHeaders();
		std::map<headerType, std::string> headers = reqProcces.getHeaders();
		for (std::map<headerType, std::string>::iterator it=headers.begin(); it!=headers.end(); it++)
			switch (it->first) {
				case ACCEPT_LANGUAGE:
					REQUIRE(it->second == "en-US,en;q=0.5");
					break;
				case HOST:
					REQUIRE(it->second == "localhost:8080");
					break;
				case LOCATION:
					REQUIRE(it->second == "Amsterdam");
					break;
				case USER_AGENT:
					REQUIRE(it->second == "Mozilla/5.0 (Macintosh; Intel Mac OS X 10.14; rv:68.0) Gecko/20100101 Firefox/68.0");
					break;
				default:
					REQUIRE(headers.empty());
			}
	}

	SECTION( "correct input 3" ) {
		std::string request = "Host:    localhost:8080\r\n"
							  "User-Agent:    Mozilla/5.0 (Macintosh; Intel Mac OS X 10.14; rv:68.0) Gecko/20100101 Firefox/68.0\r\n"
							  "location:   Amsterdam\r\n"
							  "Accept-Language:en-US,en;q=0.5\r\n";

		reqProcces.setRawRequest(request);

		reqProcces.parseHeaders();
		std::map<headerType, std::string> headers = reqProcces.getHeaders();
		for (std::map<headerType, std::string>::iterator it=headers.begin(); it!=headers.end(); it++)
			switch (it->first) {
				case ACCEPT_LANGUAGE:
					REQUIRE(it->second == "en-US,en;q=0.5");
					break;
				case HOST:
					REQUIRE(it->second == "localhost:8080");
					break;
				case LOCATION:
					REQUIRE(it->second == "Amsterdam");
					break;
				case USER_AGENT:
					REQUIRE(it->second == "Mozilla/5.0 (Macintosh; Intel Mac OS X 10.14; rv:68.0) Gecko/20100101 Firefox/68.0");
					break;
				default:
					REQUIRE(headers.empty());
			}
	}

	SECTION( "correct input 4" ) {
		std::string request = "Host:    localhost:8080  \r\n"
							  "User-Agent:    Mozilla/5.0 (Macintosh; Intel Mac OS X 10.14; rv:68.0) Gecko/20100101 Firefox/68.0\r\n"
							  "location:   Amsterdam  \r\n"
							  "Accept-Language:en-US,en;q=0.5\r\n\r\n";
		reqProcces.setRawRequest(request);

		reqProcces.parseHeaders();
		std::map<headerType, std::string> headers = reqProcces.getHeaders();
		for (std::map<headerType, std::string>::iterator it=headers.begin(); it!=headers.end(); it++)
			switch (it->first) {
				case ACCEPT_LANGUAGE:
					REQUIRE(it->second == "en-US,en;q=0.5");
					break;
				case HOST:
					REQUIRE(it->second == "localhost:8080");
					break;
				case LOCATION:
					REQUIRE(it->second == "Amsterdam");
					break;
				case USER_AGENT:
					REQUIRE(it->second == "Mozilla/5.0 (Macintosh; Intel Mac OS X 10.14; rv:68.0) Gecko/20100101 Firefox/68.0");
					break;
				default:
					REQUIRE(headers.empty());
			}
	}

	SECTION( "correct input 5" ) {
		std::string request = "Host:    localhost:8080  \r\n"
							  "DNT: 1\n"
							  "location:   Amsterdam  \r\n"
							  "Accept-Language:en-US,en;q=0.5\r\n"
							  "User-Agent:    Mozilla/5.0 (Macintosh; Intel Mac OS X 10.14; rv:68.0) Gecko/20100101 Firefox/68.0\r\n"
							  "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8\r\n"
							  "Accept-Encoding: gzip, deflate\r\n"
							  "Connection: keep-alive\r\n"
							  "Upgrade-Insecure-Requests: 1\r\n"
							  "Pragma: no-cache\r\n"
							  "Cache-Control: no-cache\r\n\r\n";
		reqProcces.setRawRequest(request);

		reqProcces.parseHeaders();
		std::map<headerType, std::string> headers = reqProcces.getHeaders();
		for (std::map<headerType, std::string>::iterator it=headers.begin(); it!=headers.end(); it++)
			switch (it->first) {
				case ACCEPT_LANGUAGE:
					REQUIRE(it->second == "en-US,en;q=0.5");
					break;
				case HOST:
					REQUIRE(it->second == "localhost:8080");
					break;
				case LOCATION:
					REQUIRE(it->second == "Amsterdam");
					break;
				case USER_AGENT:
					REQUIRE(it->second == "Mozilla/5.0 (Macintosh; Intel Mac OS X 10.14; rv:68.0) Gecko/20100101 Firefox/68.0");
					break;
				default:
					REQUIRE(headers.empty());
			}
	}

	SECTION( "incorrect input 1" ) {
		std::string request = "Host :    localhost:8080  \r\n"
							  "User-Agent:    Mozilla/5.0 (Macintosh; Intel Mac OS X 10.14; rv:68.0) Gecko/20100101 Firefox/68.0\r\n"
							  "location:   Amsterdam  \r\n"
							  "Accept-Language:en-US,en;q=0.5\r\n\r\n";
		reqProcces.setRawRequest(request);

		REQUIRE_THROWS(reqProcces.parseHeaders() );
	}

	SECTION( "incorrect input 2" ) {
		std::string request = "Host: localhost:8080\r\n"
							  "User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10.14; rv:68.0) Gecko/20100101 Firefox/68.0\r\n"
							  "location: \r\n"
							  "Accept-Language:en-US,en;q=0.5\r\n\r\n";
		reqProcces.setRawRequest(request);

		REQUIRE_THROWS(reqProcces.parseHeaders() );
	}

	SECTION( "incorrect input 3" ) {
		std::string request = "Host: localhost:8080\r\n"
							  "User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10.14; rv:68.0) Gecko/20100101 Firefox/68.0\r\n"
							  "location: AMS\r\n"
							  " Accept-Language:en-US,en;q=0.5\r\n\r\n";
		reqProcces.setRawRequest(request);

		REQUIRE_THROWS(reqProcces.parseHeaders() );
	}

	SECTION( "incorrect input 4" ) {
		std::string request = "Host: localhost:8080\r\n"
							  "User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10.14; rv:68.0) Gecko/20100101 Firefox/68.0\r\n"
							  "loca tion: AMS\r\n"
							  "Accept-Language:en-US,en;q=0.5\r\n\r\n";
		reqProcces.setRawRequest(request);

		REQUIRE_THROWS(reqProcces.parseHeaders() );
	}

	SECTION( "incorrect input 5" ) {
		std::string request = "\r\nHost: localhost:8080\r\n"
							  "User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10.14; rv:68.0) Gecko/20100101 Firefox/68.0\r\n"
							  "location: AMS\r\n"
							  "Accept-Language:en-US,en;q=0.5\r\n\r\n";
		reqProcces.setRawRequest(request);

		REQUIRE_NOTHROW(reqProcces.parseHeaders() );
	}
}