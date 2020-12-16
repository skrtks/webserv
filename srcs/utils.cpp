//
// Created by peerdb on 09-12-20.
//

#include <string>
#include <iostream>

namespace ft {

	std::string getextension(const std::string &uri) {
		size_t dotindex = uri.rfind('.');
		if (dotindex == std::string::npos)
			return "";
		return uri.substr(dotindex, uri.find_first_of('/', dotindex) - dotindex);
	}

	std::string inttostring(int n) {
		std::string ss;

		while (n) {
			char i = '0' + (n % 10);
			n /= 10;
			ss = i + ss;
		}
		return ss;
	}
}