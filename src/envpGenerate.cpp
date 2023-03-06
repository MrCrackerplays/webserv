//
//  envpGenerate.cpp
//  server_xcode_project
//
//  Created by Julia Demura on 06/03/2023.
//

#include "envpGenerate.hpp"
#include "parseRequest.hpp"

const char **envpGenerate(parsRequest request){
	
	const char *envp[17];
	std::string str;
	
	
	//always same, hardcoded
	str = "SERVER_SOFTWARE=webserv/1.0";
	envp[0] = str.c_str();
	str = "GATEWAY_INTERFACE=CGI/1.1";
	envp[1] = str.c_str();
	
	
	//generated
	str = "SERVER_NAME=";
	envp[2] = str.c_str();
	
	
	
	
	
	return envp;
}
