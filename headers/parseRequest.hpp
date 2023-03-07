//
//  parseRequest.hpp
//  server_xcode_project
//
//  Created by Julia Demura on 16/02/2023.
//

#ifndef parseRequest_hpp
#define parseRequest_hpp

#include <stdio.h>
#include <string>
#include <map>
#include <vector>

typedef enum {
	
	GET = 0,
	POST = 1,
	DELETE = 2,
	NOTSPECIFERR = 3
	
}	method;

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

typedef struct {
	
	method method;
	std::string physicalPathCgi;
	size_t contentLenght;
	std::string methodString;
	std::string urlPath;
	std::string httpVers;
	
	std::string body;
	
	std::map<std::string, std::vector<std::string>> query;
	std::map<std::string, std::vector<std::string>> headers;
	std::string hostNameHeader;
	std::string queryString;
	
	
	codeStatus status;
	//need to think about the rest if recv return an ugly edge of buffer
}	parsRequest;

void parseRequest(std::string parsBuff);
bool isDirectory(std::string& path);
bool isFile(std::string& path);
bool ifFileExsist(std::string &path);
std::string getHeaderByKey(std::map<std::string, std::vector<std::string>>& headers, const std::string& key);

#endif /* parseRequest_hpp */
