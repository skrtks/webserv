//
// Created by Peer De bakker on 11/1/20.
//

#ifndef WEBSERV_BASE64_H
# define WEBSERV_BASE64_H
# include <string>

static inline bool is_base64(unsigned char c);
std::string base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len);
std::string base64_decode(std::string const& encoded_string);
#endif //WEBSERV_BASE64_H
