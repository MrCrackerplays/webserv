//
//  parseRequest.cpp
//  server_xcode_project
//
//  Created by Julia Demura on 16/02/2023.
//

#include "parseRequest.hpp"
#include "Server.hpp"
#include <iostream>
#include <sstream>
#include "autoindex.hpp"

//https://cplusplus.com/reference/sstream/istringstream/

std::string	getQueryParams(std::string &path, std::map<std::string, std::vector<std::string> >& requestQuery){
	
	std::string queryString;
	
	std::string::size_type pos = path.find('?');
	std::string extractedQueryParams;
	if (pos != std::string::npos){ //instead of npos size_type(-1) can be used
		extractedQueryParams = path.substr(pos + 1);
		path.erase(pos);
		std::istringstream queryStream(extractedQueryParams);
		std::string parameter;
		while(std::getline(queryStream, parameter, '&')) {
			queryString += parameter; ///create queryString for envp further
			std::string::size_type equalsPos = parameter.find('=');
			if (equalsPos != std::string::npos) { //instead of npos
				std::string key = parameter.substr(0, equalsPos);
				std::string value = parameter.substr(equalsPos + 1);
				requestQuery[key].push_back(value);
			}
		}
	}
	return queryString;
}

void	packHeaderInMap(std::string& headerName, std::string& headerBody, std::map<std::string, std::vector<std::string> >& headers){
	
	std::map<std::string, std::vector<std::string> >::iterator it = headers.find(headerName);
		if (it != headers.end()) {
			it->second.push_back(headerBody);
		} else {
			headers[headerName].push_back(headerBody);
		}
}

std::string	getHeaders(std::istringstream& requestStream, std::map<std::string, std::vector<std::string> >& headers){
	
	std::string hostNameHeader;
	std::string line;
	
	while (std::getline(requestStream, line) && line != "\r\n\r\n") {
		
		size_t pos = line.find(": ");
		if (pos != std::string::npos) {
			//std::cout << line << std::endl;
			if (!line.empty() && line.back() == '\r') {
				line.pop_back();
			}
			std::string headerName = line.substr(0, pos);
			std::string headerBody = line.substr(pos + 2);
			size_t	pos2 = headerBody.rfind(":");
			if (pos2 != std::string::npos) {
				headerBody = headerBody.substr(0, pos2);
			}
			if (headerName == "Host") {
				hostNameHeader = headerBody;
			}
			packHeaderInMap(headerName, headerBody, headers);
		}
	}
	if (!hostNameHeader.empty() && hostNameHeader.back() == '\r') {
		hostNameHeader.pop_back();
	}
	return hostNameHeader;
}

method	getMethodFromRequest(std::string& method){
	if (method.empty()) {
		return ERR;
	}else if (method.compare("GET") == 0) {
		return GET;
	} else if (method.compare("POST") == 0) {
		return POST;
	} else if (method.compare("DELETE") == 0) {
		return DELETE;
	}
	return NOTSUPPORTED;
}

void	physicalPathMagic(std::string& physicalPathCgi, bool& autoindex, std::string& path, const Location& closestLocation) {
	// in case it finishes on '/' I need to add 1 more thing
	if (physicalPathCgi[physicalPathCgi.length() - 1] == '/'){
		
		if (closestLocation.getDirectoryListing()){
			//if there is autoindex on instead of get default file we need to generate our own
			std::string autoindexstring = generate_autoindex(physicalPathCgi, path);
			//this is the body of request -> content, it is not a name
			//HANDLE
			autoindex = true;
		} else {
			physicalPathCgi += closestLocation.getDefaultFile();
		}
	}
}

std::string getFileFromAnyServer(std::map<std::string, std::vector<Server> >& servers, std::string& hostPort, std::string& hostNameHeader, std::string& url, bool &autoindex){
	
	std::string physicalPathCgi;
	std::vector<Server> serversVect = servers[hostPort];
	std::vector<Server>::iterator it;
	if (hostNameHeader.length()) {
		for (it = serversVect.begin(); it < serversVect.end(); it++) {
			Server & local = *it;
			for (std::vector<const std::string>::iterator it2 = local.getNames().begin(); it2 < local.getNames().end(); it2++) {
				std::string str = *it2;
				//std::cout <<str<<std::endl;
				if (str == hostNameHeader) {
					const Location & closestLocation = local.getClosestLocation(url);
					std::string root = closestLocation.getRoot();
					std::string path = closestLocation.getPath();
					physicalPathCgi = root + url.substr(path.length());
					physicalPathMagic(physicalPathCgi, autoindex, path, closestLocation);
					return physicalPathCgi;
				}
			}
		}
	}
	const Location & closestLocation = serversVect.at(0).getClosestLocation(url);
	std::string root = closestLocation.getRoot();
	std::string path = closestLocation.getPath();
	physicalPathCgi = root + url.substr(path.length());
	physicalPathMagic(physicalPathCgi, autoindex, path, closestLocation);
	return physicalPathCgi;
}

Server & getServer(std::map<std::string, std::vector<Server> > &servers, std::string& hostPort, std::string& hostNameHeader){
	
	std::vector<Server> &serversVect = servers[hostPort];
	
	if (hostNameHeader.length()) {
		for (std::vector<Server>::iterator it = serversVect.begin(); it < serversVect.end(); it++) {
			Server & local = *it;
			for (std::vector<const std::string>::iterator it2 = local.getNames().begin(); it2 < local.getNames().end(); it2++) {
				std::string str = *it2;
				if (str == hostNameHeader){
					return local;
				}
			}
		}
	}
	return serversVect.front();
}

void findMethodInServer(parsRequest &request, std::map<std::string, std::vector<Server> > &servers, std::string& hostPort){
	
	Location location = getServer(servers, hostPort, request.hostNameHeader).getClosestLocation(request.urlPath);
	std::vector<std::string> methods = location.getMethods();
	std::vector<std::string> cgis = location.getCGIs();
	
	
	if (std::find(methods.begin(), methods.end(), request.methodString) == methods.end()){
		request.code = 405;// Method Not Allowed
		request.callCGI = false;
	} else {
		request.callCGI = false;
		std::string	file = request.physicalPathCgi.substr(request.physicalPathCgi.rfind('/'));
		size_t pos = file.rfind('.');
		if (pos != std::string::npos){
			std::string	extension = file.substr(pos);
			if (std::find(cgis.begin(), cgis.end(), extension) != cgis.end())
				request.callCGI = true;
		}
	}
}

//check code list in codes.cpp
bool	isBadRequest(parsRequest& request){
	
	if (request.method == NOTSUPPORTED) {
		request.code = 501;//Not Implemented
		return true;
	} else if (request.method == ERR) {
		request.code = 405;//Method Not Allowed
		return true;
	} else if (request.urlPath.empty() || request.httpVers.empty()) {
		request.code = 400;//check what code
		return true;
		
	} else if (request.httpVers != "HTTP/1.1" && request.httpVers != "HTTP/1.0") {
		request.code = 505;//HTTP Version Not Supported
		return true;
	}
	return false;
}

bool	isRedirection(parsRequest &request, std::map<std::string, std::vector<Server> > &servers, std::string& hostPort){
	
	Location loc = getServer(servers, hostPort, request.hostNameHeader).getClosestLocation(request.urlPath);
	std::pair<std::string, std::string> redirectPair = loc.getRedirect();
	
	if (redirectPair.first != "") {
		request.newlocation = redirectPair.second;
		request.redirect = true;
		request.code = std::stoi(redirectPair.first);
		return true;
	}
	return false;
}

bool	cookieEnforcement(parsRequest &request, std::map<std::string, std::vector<Server> > &servers, std::string& hostPort){
	
	std::string cookie = getServer(servers, hostPort, request.hostNameHeader).getClosestLocation(request.urlPath).getRequireCookie();
	//check if there's a cookie requirement
	if (cookie == "")
		return false;

	//check if there's a cookie header
	if (request.headers.find("Cookie") == request.headers.end()) {
		request.code = 403;
		// std::cout << "couldnt find cookie header" << std::endl;
		return true;
	}

	std::vector<std::string> vec = request.headers["Cookie"];
	//check if the required cookie is in the header
	bool	found = false;
	for (std::vector<std::string>::iterator it = vec.begin(); it < vec.end(); it++) {
		std::string str = *it;
		if (str.find(cookie) != std::string::npos) {
			found = true;
			break;
		}
	}
	if (!found) {
		request.code = 403;
		// std::cout << "couldnt find cookie inside cookie header" << std::endl;
		return true;
	}
	return false;
}

parsRequest parseRequest(std::string requestBuff, std::map<std::string, std::vector<Server> > &servers, std::string& hostPort){
	
	parsRequest request;
	std::istringstream requestStream(requestBuff);
	
	request.code = 200;
	requestStream >> request.methodString;
	request.method = getMethodFromRequest(request.methodString);
	requestStream >> request.urlPath >> request.httpVers;
	if (isBadRequest(request)){
		return request;
	} else {
		request.queryString = getQueryParams(request.urlPath, request.query);
		request.hostNameHeader = getHeaders(requestStream, request.headers);

		if (cookieEnforcement(request, servers, hostPort)) {
			return request;
		}

		if (isRedirection(request, servers, hostPort)){
			return request;
		}
		
		request.physicalPathCgi = getFileFromAnyServer(servers, hostPort, request.hostNameHeader, request.urlPath, request.autoindex);
		if (request.autoindex == true){
			request.requestBody = request.physicalPathCgi;
		} else {
			requestStream >> request.requestBody;
		}
		findMethodInServer(request, servers, hostPort);
		if (request.method != GET && request.autoindex == true){ //???? not sure
			request.code = 405;
		}
	}
	return request;
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
