/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   split.cpp                                          :+:    :+:            */
/*                                                     +:+                    */
/*   By: pde-bakk <pde-bakk@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/10/08 16:11:55 by pde-bakk      #+#    #+#                 */
/*   Updated: 2020/10/08 17:55:50 by pde-bakk      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include <string>
#include <iostream>
#include <vector>

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

// int main( ) {
// 	std::string s;
// 	s = "hasta \t la vista\n motherfucker\t\n    bitch  ";
// 	std::vector<std::string> vec;
// 	vec = split(s, " \t\n");
// 	for (size_t i = 0; i < vec.size(); i++)
// 		std::cout << "found: \"" << vec[i] << "\"." << std::endl;
// 	s = "  /  {";
// 	vec = split(s, " \t\n");
// 	for (size_t i = 0; i < vec.size(); i++)
// 		std::cout << "found: \"" << vec[i] << "\"." << std::endl;
// 	s = "{";
// 	vec = split(s, " \t\n");
// 	for (size_t i = 0; i < vec.size(); i++)
// 		std::cout << "found: \"" << vec[i] << "\"." << std::endl;
// }
