//
//  constructResponse.hpp
//  server_xcode_project
//
//  Created by Julia Demura on 15/02/2023.
//

#ifndef constructResponse_hpp
#define constructResponse_hpp

#include "utilsMethods.hpp"
#include <string>
#include <fstream>
#include <iostream>
#include <sys/socket.h>

typedef struct {
	method method;
	size_t contentLenght;
	std::string contentType;
	std::string codeMessage;
	std::string errorPageByCode;
	
	std::string body;
	std::string header;
	
	std::string cgiReply;
	std::string headerAfterCgi;
} response;

std::string	formResponseString(response response, std::string& filename);

#endif /* constructResponse_hpp */
