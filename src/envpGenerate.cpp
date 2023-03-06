//
//  envpGenerate.cpp
//  server_xcode_project
//
//  Created by Julia Demura on 06/03/2023.
//

#include "envpGenerate.hpp"
#include "parseRequest.hpp"
#include <string>

//typedef struct {
//
//	std::string hostName;
//	std::string portNumber;
//	parsRequest request;
//
//}dataForProcess;

const char **envpGenerate(parsRequest request, std::string portNumberSocket, std::string hostNameSocket){
	
	const char *envp[17];
	std::string str;
	
	
	
	//always same, hardcoded
	str = "SERVER_SOFTWARE=webserv/1.0";
	envp[0] = str.c_str();
	str = "GATEWAY_INTERFACE=CGI/1.1";
	envp[1] = str.c_str();
	
	//generated
	
	//maybe could just use hostNameSocket?
	size_t pos = request.urlPath.find('/');
	std::string hostName = request.urlPath.substr(0, pos);
	str = "SERVER_NAME=" + hostName;
	envp[2] = str.c_str();
	// end of doubts
	str = "SERVER_PROTOCOL=" + request.httpVers;
	envp[3] = str.c_str();
	str = "SERVER_PORT=" + portNumberSocket;
	envp[4] = str.c_str();
	str = "REQUEST_METHOD=" + request.methodString;
	envp[5] = str.c_str();
	str = "QUERY_STRING=" + request.queryString;
	envp[6] = str.c_str();
	
	
	return envp;
}
