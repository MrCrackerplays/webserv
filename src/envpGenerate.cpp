//
//  envpGenerate.cpp
//  server_xcode_project
//
//  Created by Julia Demura on 06/03/2023.
//

#include "envpGenerate.hpp"

char **envpGenerate(parsRequest request, std::string portNumberSocket, std::string hostNameSocket){
	
	char **envp;
	
	envp = (char **)malloc(sizeof(char *) * 17);
	if (envp == NULL)
		return NULL;
	
	std::string str0 = "SERVER_SOFTWARE=webserv/1.0";
	envp[0] = (char *)str0.c_str();
	
	std::string str1 = "GATEWAY_INTERFACE=CGI/1.1";
	envp[1] = (char *)str1.c_str();
	size_t pos = request.urlPath.find('/');
	if (pos == std::string::npos){
		pos = 0;
	}
	std::string hostName = request.urlPath.substr(0, pos);
	std::string str2 = "SERVER_NAME=" + hostName;
	envp[2] = (char *)str2.c_str();
	std::string str3 = "SERVER_PROTOCOL=" + request.httpVers;
	envp[3] = (char *)str3.c_str();
	std::string str4 = "SERVER_PORT=" + portNumberSocket;
	envp[4] = (char *)str4.c_str();
	std::string str5 = "REQUEST_METHOD=" + request.methodString;
	envp[5] = (char *)str5.c_str();
	std::string str6 = "QUERY_STRING=" + request.queryString;
	envp[6] = (char *)str6.c_str();
	
	
	//FIX - check if the cont type is correct name for header
	std::string contType = getHeaderByKey(request.headers, "content-type");
	std::string str7 = "CONTENT_TYPE=" + contType; //content-type in the header
	envp[7] = (char *)str7.c_str();
	//FIX
	
	std::string str8 = "CONTENT_LENGTH=" + std::to_string(request.contentLenght);
	envp[8] = (char *)str8.c_str();
	pos = request.urlPath.find('/');
	if (pos == std::string::npos){
		pos = 0;
	}
	std::string scriptN = request.urlPath.substr(pos+1, request.urlPath.length());
	std::string str9 = "SCRIPT_NAME=" + scriptN;
	envp[9] = (char *)str9.c_str();
	std::string str10 = "PATH_INFO=" + request.hostNameHeader;
	envp[10] = (char *)str10.c_str();
	std::string str11 = "PATH_TRANSLATED="  + request.hostNameHeader;
	envp[11] = (char *)str11.c_str();
	std::string str12 = "REMOTE_HOST=";
	envp[12] = (char *)str12.c_str();
	std::string str13 = "REMOTE_ADDR=";
	envp[13] = (char *)str13.c_str();
	std::string str14 = "AUTH_TYPE=";
	envp[14] = (char *)str14.c_str();
	std::string str15 = "REMOTE_USER=";
	envp[15] = (char *)str15.c_str();
	std::string str16 = "REMOTE_IDENT=";
	envp[16] = (char *)str16.c_str();
	envp[17] = NULL;
	return envp;
}
