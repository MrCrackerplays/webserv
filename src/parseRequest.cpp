//
//  parseRequest.cpp
//  server_xcode_project
//
//  Created by Julia Demura on 16/02/2023.
//

#include "parseRequest.hpp"

#include <iostream>
#include <sstream>
//https://cplusplus.com/reference/sstream/istringstream/

void	getQueryParams(std::string &path, std::map<std::string, std::vector<std::string>> & requestQuery){
	
	std::string::size_type pos = path.find('?');
	std::string extractedQueryParams;
	if (pos != std::string::npos){ //instead of npos size_type(-1) can be used
		extractedQueryParams = path.substr(pos + 1);
		path.erase(pos);
		std::istringstream queryStream(extractedQueryParams);
		std::string parameter;
		
		while(std::getline(queryStream, parameter, '&')){
			std::string::size_type equalsPos = parameter.find('=');
			if (equalsPos != std::string::npos){ //instead of npos
				std::string key = parameter.substr(0, equalsPos);
				std::string value = parameter.substr(equalsPos + 1);
				requestQuery[key].push_back(value);
			}
		}
	}
}

void	packHeaderInMap(std::string& headerName, std::string& headerBody, std::map<std::string, std::vector<std::string>>& headers){
	
	//auto it = headers.find(headerName);
	std::map<std::string, std::vector<std::string>>::iterator it = headers.find(headerName);
		if (it != headers.end()) {
			it->second.push_back(headerBody);
		} else {
			headers[headerName].push_back(headerBody);
		}
}

void	getHeaders(std::istringstream& requestStream, std::map<std::string, std::vector<std::string>>& headers){
	
	std::string line;
	while (std::getline(requestStream, line) && line != "\r"){
		
		size_t pos = line.find(": ");
		if (pos != std::string::npos){
			
			std::string headerName = line.substr(0, pos);
			std::string headerBody = line.substr(pos + 2);
			packHeaderInMap(headerName, headerBody, headers);
		}
	}
}

method	getMethod(std::istringstream & requestStream){
	
	std::string method;
	requestStream >> method;
	
	if (method.empty()){
		return NOTSPECIFERR;
	}else if (method.compare("GET") == 0){
		return GET;
	} else if (method.compare("POST") == 0) {
		return POST;
	} else if (method.compare("DELETE") == 0) {
		return DELETE;
	}
	return NOTSPECIFERR;
}


void parseRequest(char *parsBuff){
	
	parsRequest pars;
	std::string request(parsBuff);
	std::istringstream requestStream(request);
	
	pars.status = OK;
	
	pars.method = getMethod(requestStream);
	requestStream >> pars.path >> pars.httpVers;
	if (pars.method == NOTSPECIFERR || pars.path.empty() || pars.httpVers.empty()){
		pars.status = BADRQST;
		return ;
	}
	if (pars.httpVers.compare("HTTP/1.1") != 0){ //there might be other check
		pars.status = BADRQST;
		return ;
	}
	
	getQueryParams(pars.path, pars.query);
	//if the path contains directory not sure how and when to check access to it
	
	getHeaders(requestStream, pars.headers);
}

