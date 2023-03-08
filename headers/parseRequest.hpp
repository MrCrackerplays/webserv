//
//  parseRequest.hpp
//  server_xcode_project
//
//  Created by Julia Demura on 16/02/2023.
//

#ifndef parseRequest_hpp
#define parseRequest_hpp

#include "utilsMethods.hpp"
#include "codes.hpp"

typedef enum {
	
	GET = 0,
	POST = 1,
	DELETE = 2,
	NOTSPECIFERR = 3
	
}	method;

typedef struct {
	
	method method;
	std::string physicalPathCgi;
	size_t contentLenght;
	std::string methodString;
	std::string urlPath;
	std::string httpVers;
	
	std::string body;
	
	std::map <std::string, std::vector<std::string> > query;
	std::map <std::string, std::vector<std::string> > headers;
	std::string hostNameHeader;
	std::string queryString;
	
	
	codeStatus status;
	//need to think about the rest if recv return an ugly edge of buffer
}	parsRequest;

void parseRequest(std::string parsBuff);
std::string getHeaderByKey(std::map <std::string, std::vector<std::string> >& headers, const std::string& key);

#endif /* parseRequest_hpp */
