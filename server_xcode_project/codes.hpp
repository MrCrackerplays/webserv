//
//  codes.hpp
//  server_xcode_project
//
//  Created by Julia Demura on 06/03/2023.
//

#ifndef codes_hpp
#define codes_hpp

#include <iostream>
#include <string>

//not sure it is even needed, maybe better to keep it simple int
//typedef enum {
//
//	INFO = 100,
//
//	OK = 200,
//	CREATED = 201,
//	NOCONTENT = 204,
//
//	REDIRECTION = 300,
//
//	BADRQST = 400,
//	UNAUTHORZD = 401,
//	FORBIDDEN = 403,
//	NOTFOUND = 404,
//
//	INTERNALSERVERR = 500
//
//}	codeStatus;

void	codes(int codeStatus, std::string &codeMessage);

#endif /* codes_hpp */
