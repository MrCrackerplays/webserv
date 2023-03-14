//
//  parseRequest.hpp
//  server_xcode_project
//
//  Created by Julia Demura on 16/02/2023.
//

#ifndef parseRequest_hpp
#define parseRequest_hpp

#include "utilsMethods.hpp"
#include "Server.hpp"
#include "codes.hpp"

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
	
	
	codeStatus code;
	//need to think about the rest if recv return an ugly edge of buffer
}	parsRequest;

//void parseRequest(std::string parsBuff);
parsRequest parseRequest(std::string parsBuff, std::map<std::string, std::vector<Server> > &servers, std::string port, std::string host);
std::string getHeaderByKey(std::map <std::string, std::vector<std::string> >& headers, const std::string& key);

#endif /* parseRequest_hpp */
