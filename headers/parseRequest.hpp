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
	
	int code;
	bool autoindex;
	
	//from request:
	std::string parsBuff;
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
	std::string save_location;
	std::string physicalPathCgi;
	std::map<std::string, std::string>	ErrorPages;
	bool callCGI;
	bool redirect;
	std::string newlocation;
}	parsRequest;

parsRequest parseRequest(std::string requestBuff, std::map<std::string, std::vector<Server> > &servers, std::string& hostPort);
std::string getHeaderByKey(std::map <std::string, std::vector<std::string> >& headers, const std::string& key);
Server & getServer(std::map<std::string, std::vector<Server> > &servers, std::string& hostPort, std::string& hostNameHeader);

#endif /* parseRequest_hpp */
