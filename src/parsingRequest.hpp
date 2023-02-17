//
//  parsingRequest.hpp
//  server_xcode_project
//
//  Created by Julia Demura on 16/02/2023.
//

#ifndef parsingRequest_hpp
#define parsingRequest_hpp

#include <stdio.h>

typedef enum {
	
	GET = 0,
	POST = 1,
	DELETE = 2
	
}	request;

typedef enum {
	
	INFO = 100,
	OK = 200,
	CREATED = 201,
	NOCONTENT = 204,
	MOVEDPERM = 301,
	FOUND = 302,
	NOTMODIF = 304,
	BADRQST = 400,
	UNAUTHORZD = 401,
	NOTFOUND = 404

}	codeStatus;

#endif /* parsingRequest_hpp */
