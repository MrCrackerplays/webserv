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
	
	size_t contentLenght; //specify what content, request or response
	std::string body; //same, specify
	
	std::map <std::string, std::vector<std::string> > query;
	std::string queryString;
	std::map <std::string, std::vector<std::string> > headers;
	std::string hostNameHeader;
	std::string methodString;
	method method;
	std::string urlPath;
	std::string httpVers;
	codeStatus code;
	bool callCGI;
	
	
	//from location:
	std::string physicalPathCgi;
	std::map<std::string, std::string>	ErrorPages;
//	std::vector<std::string>			methods;
//	std::vector<std::string>			cgis;
}	parsRequest;

parsRequest parseRequest(std::string parsBuff, std::map<std::string, std::vector<Server> > &servers, std::string port, std::string host);
std::string getHeaderByKey(std::map <std::string, std::vector<std::string> >& headers, const std::string& key);
Server & getServer(std::map<std::string, std::vector<Server> > &servers, std::string& hostPort, std::string& hostNameHeader);

#endif /* parseRequest_hpp */
