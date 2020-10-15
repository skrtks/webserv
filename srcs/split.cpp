/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   split.cpp                                          :+:    :+:            */
/*                                                     +:+                    */
/*   By: pde-bakk <pde-bakk@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/10/08 16:11:55 by pde-bakk      #+#    #+#                 */
/*   Updated: 2020/10/15 20:11:20 by pde-bakk      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include <string>
#include <vector>
#include <iostream>

namespace ft {
	
	std::vector<std::string>	split(const std::string& s, const std::string& delim) {
		size_t start, end = 0;
		std::vector<std::string> vec;

		while (end != std::string::npos) {
			start = s.find_first_not_of(delim, end);
			end = s.find_first_of(delim, start);
			if (end != std::string::npos)
				vec.push_back(s.substr(start, end - start));
			else if (start != std::string::npos)
				vec.push_back(s.substr(start, s.back() - start));
		}
		return vec;
	}

std::string inttostring(int a) {
    std::string ss="";   //create empty string
    while(a)
    {
        int x=a%10;
        a/=10;
        char i='0';
        i=i+x;
        ss=i+ss;      //append new character at the front of the string!
    }
    return ss;
}

}
