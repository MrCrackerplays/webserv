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
	
	//codeStatus code;
	int code;
	
	//from request:
	std::map <std::string, std::vector<std::string> > query;
	std::string queryString;
	std::map <std::string, std::vector<std::string> > headers;
	std::string hostNameHeader;
	std::string methodString;
	method method;
	std::string urlPath;
	std::string httpVers;
	std::string requestBody;
	size_t requestBodyLen;
	
	//from location:
	std::string physicalPathCgi;
	std::map<std::string, std::string>	ErrorPages;
	bool callCGI;
}	parsRequest;

parsRequest parseRequest(std::string requestBuff, std::map<std::string, std::vector<Server> > &servers, std::string port, std::string host);
std::string getHeaderByKey(std::map <std::string, std::vector<std::string> >& headers, const std::string& key);
Server & getServer(std::map<std::string, std::vector<Server> > &servers, std::string& hostPort, std::string& hostNameHeader);

#endif /* parseRequest_hpp */
