//
//  envpGenerate.hpp
//  server_xcode_project
//
//  Created by Julia Demura on 06/03/2023.
//

#ifndef envpGenerate_hpp
#define envpGenerate_hpp

#include "parseRequest.hpp"
#include <string>
#include <unistd.h>

char **envpGenerate(parsRequest request, std::string portNumberSocket, std::string hostNameSocket);


#endif /* envpGenerate_hpp */
