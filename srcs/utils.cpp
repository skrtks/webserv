//
// Created by peerdb on 09-12-20.
//

#include "libftGnl.hpp"
#include <string>

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
	void	stringtoupper(std::string& str) {
		for (size_t i = 0; i < str.length(); ++i) {
			str[i] = ft_toupper(str[i]);
		}
	}

	void	trimstring(std::string& str, const char* totrim) {
		size_t	begin = str.find_first_not_of(totrim),
				end = str.find_last_not_of(totrim);
		if (begin == std::string::npos || end == 0)
			return;
		str = str.substr(begin, end - begin + 1);
	}
}
