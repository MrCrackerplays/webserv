//
//  constructResponse.hpp
//  server_xcode_project
//
//  Created by Julia Demura on 15/02/2023.
//

#ifndef constructResponse_hpp
#define constructResponse_hpp

#include "parseRequest.hpp"
#include <string>
#include <fstream>
#include <iostream>
#include <sys/socket.h>

//typedef struct {
//	
//	std::string header;
//	std::string body;
//	std::string codeMessage;
//	codeStatus codeStatus;
//	
//} response;

std::string	formResponseString(method type, int codeStatus, std::string& filename);

#endif /* constructResponse_hpp */
