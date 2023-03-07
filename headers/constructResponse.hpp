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

std::string	formResponseString(method type, int codeStatus, std::string& filename);
std::string pickCodeMessage(int codeStat);

#endif /* constructResponse_hpp */
