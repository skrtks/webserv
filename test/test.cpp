#define CATCH_CONFIG_MAIN
#include "Catch2.h"
#include "../ProcessRequest.hpp"

TEST_CASE( "Parsing request", "[Request]" ) {

	ProcessRequest reqProcces;

	SECTION( "Correct input 1" ) {
		std::string request = "GET /index.html HTTP/1.1\r\nData: bla\r\n";
		reqProcces.setRawRequest(request);

		reqProcces.processRequestLine();

		REQUIRE(reqProcces.getMethod() == GET);
		REQUIRE(reqProcces.getUri() == "/index.html");
		std::pair<int, int> version(1, 1);
		REQUIRE(reqProcces.getVersion() == version);
	}
	SECTION( "Correct input 2" ) {
		std::string request = "POST /index HTTP/0.9\r\n";
		reqProcces.setRawRequest(request);

		reqProcces.processRequestLine();

		REQUIRE(reqProcces.getMethod() == POST);
		REQUIRE(reqProcces.getUri() == "/index");
		std::pair<int, int> version(0, 9);
		REQUIRE(reqProcces.getVersion() == version);
	}
	SECTION( "Correct input 3" ) {
		std::string request = "HEAD / HTTP/1.0\r\n";
		reqProcces.setRawRequest(request);

		reqProcces.processRequestLine();

		REQUIRE(reqProcces.getMethod() == HEAD);
		REQUIRE(reqProcces.getUri() == "/");
		std::pair<int, int> version(1, 0);
		REQUIRE(reqProcces.getVersion() == version);
	}
	SECTION( "incorrect input 1" ) {
		std::string request = "HEA / HTTP/1.0\r\n";
		reqProcces.setRawRequest(request);

		REQUIRE_THROWS_WITH( reqProcces.processRequestLine(), "Incorrect value for method" );
	}
	SECTION( "incorrect input 2" ) {
		std::string request = "HEAD/ HTTP/1.0\r\n";
		reqProcces.setRawRequest(request);

		REQUIRE_THROWS( reqProcces.processRequestLine() );
	}
	SECTION( "incorrect input 3" ) {
		std::string request = "HEAD /HTTP/1.0\r\n";
		reqProcces.setRawRequest(request);

		REQUIRE_THROWS( reqProcces.processRequestLine() );
	}
	SECTION( "incorrect input 4" ) {
		std::string request = "HEAD / HTTP /1.0\r\n";
		reqProcces.setRawRequest(request);

		REQUIRE_THROWS( reqProcces.processRequestLine() );
	}
	SECTION( "incorrect input 5" ) {
		std::string request = "HEAD / HTTP/1. 0\r\n";
		reqProcces.setRawRequest(request);

		REQUIRE_THROWS( reqProcces.processRequestLine() );
	}
	SECTION( "incorrect input 6" ) {
		std::string request = "HEAD / HTTP/1.0 \r\n";
		reqProcces.setRawRequest(request);

		REQUIRE_THROWS( reqProcces.processRequestLine() );
	}
	SECTION( "incorrect input 7" ) {
		std::string request = "HEAD/  HTTP/1.0\r\n";
		reqProcces.setRawRequest(request);

		REQUIRE_THROWS( reqProcces.processRequestLine() );
	}
	SECTION( "incorrect input 8" ) {
		std::string request = "  HEAD/HTTP/1.0\r\n";
		reqProcces.setRawRequest(request);

		REQUIRE_THROWS( reqProcces.processRequestLine() );
	}
	SECTION( "incorrect input 9" ) {
		std::string request = "HEAD/HTTP/1.0  \r\n";
		reqProcces.setRawRequest(request);

		REQUIRE_THROWS( reqProcces.processRequestLine() );
	}
	SECTION( "incorrect input 10" ) {
		std::string request = "HEAD/HT TP/1. 0\r\n";
		reqProcces.setRawRequest(request);

		REQUIRE_THROWS( reqProcces.processRequestLine() );
	}
	SECTION( "incorrect input 11" ) {
		std::string request = "HEAD /HTTP /1.0\r\n";
		reqProcces.setRawRequest(request);

		REQUIRE_THROWS( reqProcces.processRequestLine() );
	}
	SECTION( "incorrect input 12" ) {
		std::string request = "HEAD/ HTTP/ 1.0\r\n";
		reqProcces.setRawRequest(request);

		REQUIRE_THROWS( reqProcces.processRequestLine() );
	}
	SECTION( "incorrect input 13" ) {
		std::string request = "HEAD/ HTTP/1.0 \r\n";
		reqProcces.setRawRequest(request);

		REQUIRE_THROWS( reqProcces.processRequestLine() );
	}
	SECTION( "incorrect input 14" ) {
		std::string request = "HEAD / HTTP/1.0 \r\n";
		reqProcces.setRawRequest(request);

		REQUIRE_THROWS( reqProcces.processRequestLine() );
	}
	SECTION( "incorrect input 15" ) {
		std::string request = "HEAD / HTTP/1.0 \n";
		reqProcces.setRawRequest(request);

		REQUIRE_THROWS( reqProcces.processRequestLine() );
	}
	SECTION( "incorrect input 16" ) {
		std::string request = "HEAD / HTTP/1.0\n";
		reqProcces.setRawRequest(request);

		REQUIRE_THROWS( reqProcces.processRequestLine() );
	}
	SECTION( "incorrect input 16" ) {
		std::string request = "HEAD / HTTP/1.0";
		reqProcces.setRawRequest(request);

		REQUIRE_THROWS( reqProcces.processRequestLine() );
	}
}