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


//GET /favicon.ico HTTP/1.1
//Host: localhost:8011
//Connection: keep-alive
//sec-ch-ua: "Not_A Brand";v="99", "Google Chrome";v="109", "Chromium";v="109"
//sec-ch-ua-mobile: ?0
//User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/109.0.0.0 Safari/537.36
//sec-ch-ua-platform: "macOS"
//Accept: image/avif,image/webp,image/apng,image/svg+xml,image/*,*/*;q=0.8
//Sec-Fetch-Site: same-origin
//Sec-Fetch-Mode: no-cors
//Sec-Fetch-Dest: image
//Referer: http://localhost:8011/
//Accept-Encoding: gzip, deflate, br
//Accept-Language: en-US,en;q=0.9,uk;q=0.8,ru-RU;q=0.7,ru;q=0.6,nl;q=0.5
//
//oding: gzip, deflate, br
//Accept-Language: en-US,en;q=0.9,uk;q=0.8,ru-RU;q=0.7,ru;q=0.6,nl;q=0.5

method	getMethod(std::istringstream & requestStream){
	
	std::string method;
	requestStream >> method;
	
	if (method.compare("GET") == 0){
		return GET;
	} else if (method.compare("POST") == 0) {
		return POST;
	} else if (method.compare("DELETE") == 0) {
		return DELETE;
	}
	return NOTSPECIF;
}

void	getQueryParams(std::string &path, std::map<std::string, std::vector<std::string>> & requestQuery){
	
	std::string::size_type pos = path.find('?');
	std::string extractedQueryParams;
	if (pos != std::string::npos){
		extractedQueryParams = path.substr(pos + 1);
		path.erase(pos);
		std::istringstream queryStream(extractedQueryParams);
		std::string parameter;
		
		while(std::getline(queryStream, parameter, '&')){
			std::string::size_type equalsPos = parameter.find('=');
			if (equalsPos != std::string::npos){
				std::string key = parameter.substr(0, equalsPos);
				std::string value = parameter.substr(equalsPos + 1);
				requestQuery[key].push_back(value);
			}
		}
		
	}
	
}

void	packHeaderInMap(std::string& headerName, std::string& headerBody, std::map<std::string, std::vector<std::string>>& headers){
	
	auto it = headers.find(headerName);
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

bool isValidRequest(parsRequest request){
	//unfinished, error check
	return true;
	
}


void parseRequest(char *parsBuff){
	
	parsRequest pars;

	std::string request(parsBuff);
	std::istringstream requestStream(request);
	
	pars.status = OK;
	pars.method = getMethod(requestStream);
	requestStream >> pars.path >> pars.httpVers;
	//need to check wether i received 3 mandatory attributes
	//	switch (isValidRequest(pars)) {
	//		case true:
	//
	//			break;
	//		case false:
	//
	//			break;
	//	}
	getQueryParams(pars.path, pars.query);
	getHeaders(requestStream, pars.headers);
}

