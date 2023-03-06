//
//  parseRequest.cpp
//  server_xcode_project
//
//  Created by Julia Demura on 16/02/2023.
//

#include "parseRequest.hpp"

#include <iostream>
#include <sstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
//https://cplusplus.com/reference/sstream/istringstream/

std::string	getQueryParams(std::string &path, std::map<std::string, std::vector<std::string>> & requestQuery){
	
	std::string queryString;
	
	std::string::size_type pos = path.find('?');
	std::string extractedQueryParams;
	if (pos != std::string::npos){ //instead of npos size_type(-1) can be used
		extractedQueryParams = path.substr(pos + 1);
		path.erase(pos);
		std::istringstream queryStream(extractedQueryParams);
		std::string parameter;
		
		while(std::getline(queryStream, parameter, '&')){
			queryString += parameter; ///create queryString for envp further
			std::string::size_type equalsPos = parameter.find('=');
			if (equalsPos != std::string::npos){ //instead of npos
				std::string key = parameter.substr(0, equalsPos);
				std::string value = parameter.substr(equalsPos + 1);
				requestQuery[key].push_back(value);
			}
		}
	}
	return queryString;
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

method	getMethod(std::string method){
	
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

bool isDirectory(std::string& path) {
	
	struct stat info;
	if (stat(path.c_str(), &info) != 0) {
		// error 404?
		return false;
	}
	if (info.st_mode & S_IFDIR) {
		return true;
	} else {
		return false;
	}
}

bool isFile(std::string& path) {
	struct stat info;
	if (stat(path.c_str(), &info) != 0) {
		// error 404?
		return false;
	}
	if (info.st_mode & S_IFREG) {
		return true;
	} else {
		return false;
	}
}

bool ifFileExsist(std::string &path){
	
	if (access(path.c_str(), F_OK) == 0)
		return true;
	return false;
}

void parseRequest(std::string parsBuff){
	
	parsRequest pars;
	std::string request(parsBuff);
	std::istringstream requestStream(request);
	
	pars.status = OK;
	
	requestStream >> pars.methodString;
	pars.method = getMethod(pars.methodString);
	requestStream >> pars.urlPath >> pars.httpVers;
	if (pars.method == NOTSPECIFERR || pars.urlPath.empty() || pars.httpVers.empty()){
		pars.status = BADRQST;
		//body should be set to error file ;
	}
	if (pars.httpVers.compare("HTTP/1.1") != 0){ //there might be other check
		pars.status = BADRQST;
			//body should be set to error file ;
	}
	pars.queryString = getQueryParams(pars.urlPath, pars.query);
	if (!ifFileExsist(pars.urlPath)){
		pars.status = NOTFOUND;
	}
	//can also check here if we have a directory in path or file, not sure if needed
	
	getHeaders(requestStream, pars.headers);
}

//example
//GET /favicon.ico HTTP/1.1
//Host: localhost:8011
//Connection: keep-alive
//sec-ch-ua: "Chromium";v="110", "Not A(Brand";v="24", "Google Chrome";v="110"
//sec-ch-ua-mobile: ?0
//User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/110.0.0.0 Safari/537.36
//sec-ch-ua-platform: "macOS"
//Accept: image/avif,image/webp,image/apng,image/svg+xml,image/*,*/*;q=0.8
//Sec-Fetch-Site: same-origin
//Sec-Fetch-Mode: no-cors
//Sec-Fetch-Dest: image
//Referer: http://localhost:8011/bla
//Accept-Encoding: gzip, deflate, br
//Accept-Language: en-US,en;q=0.9,uk;q=0.8,ru-RU;q=0.7,ru;q=0.6,nl;q=0.5
//
//oding: gzip, deflate, br
//Accept-Language: en-US,en;q=0.9,uk;q=0.8,ru-RU;q=0.7,ru;q=0.6,nl;q=0.5
//
//\337o
///</html> <body>
//	<h1 align="center">information & practice<br>2 in 1</h1>
//
//	<img src="https://www.petmd.com/sites/default/files/smart-cat-incopy-483561506.jpg" height="100"/>
//
//	<h3>links:</h3>
//	<p><a href="https://github.com/MrCrackerplays/webserv">our github</a></p>
//	<p><a href="https://html.com/">html documentation</a></p>
//	<br>
//
///</body> </html>


//header : bite sent: 65 response lenght: 65
//body : bite sent: 348 response lenght: 348
//new fd after accept   5
//new fd after accept   6
//res of recv 701   for fd 5
