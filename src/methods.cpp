//
//  methods.cpp
//  server_xcode_project
//
//  Created by Julia Demura on 16/02/2023.
//

#include "methods.hpp"
#include "codes.hpp"
#include <cstdio>
#include <map>
#include <dirent.h>

void	methodGet(parsRequest& request, std::string& body){
	
	if (isDirectory(request.physicalPathCgi) == false && isFile(request.physicalPathCgi) == false){
		request.code = 404;
		return;
	} else if (ifFileReadable(request.physicalPathCgi) == false) {
		request.code = 401;
		return;
	}
	readFileBinary(request.physicalPathCgi, body);
}

static bool	isEmptyDir(std::string& path) {
	DIR *dir = opendir(path.c_str());
	if (!dir) {
		return false;
	}
	struct dirent *ent = readdir(dir);
	int i = 0;
	while (ent && i < 3) {
		i++;
		ent = readdir(dir);
	}
	return (i == 2);
}

void methodDelete(parsRequest& request) {
	if (isDirectory(request.physicalPathCgi) == false && isFile(request.physicalPathCgi) == false) {
		request.code = 404;
	} else if (ifFileWritable(request.physicalPathCgi) == false) {
		request.code = 401;
	}
	else if (isDirectory(request.physicalPathCgi) && !isEmptyDir(request.physicalPathCgi)) {
		request.code = 409;
	}
	else if (std::remove(request.physicalPathCgi.c_str()) != 0) {
		throw std::runtime_error("methods : remove");
	}
}

static void	fallback_error(response& response, int code) {
	std::ostringstream oss;
	oss << "<html><head><title>Internal Server Error</title><style>*{text-align: center;}</style></head><body><h1>An Internal Server Error has occurred while attempting to get the page for error code '" << code << "'</h1><hr>Webserv</body></html>";
	response.body = oss.str();
	response.contentLenght = response.body.length();
	response.contentType = "text/html";
}

response responseStructConstruct(std::map<std::string, std::vector<Server> > &servers, std::string& hostPort, std::string body, parsRequest& request){
	
	response response;
	if (request.code != 408){
			std::string setCookie = getServer(servers, hostPort, request.hostNameHeader).getClosestLocation(request.urlPath).getSetCookie();
		if (setCookie != ""){
			response.setCookie = setCookie;
		}
		if (request.code / 100 == 3){ //redirection
			redirectionResponse(request.code, request.newlocation, response);
			return response;
		}
	}
	
	if (request.code != 200){
		response.body = "";
		response.errorPageByCode = getServer(servers, hostPort, request.hostNameHeader).getErrorPage(std::to_string(request.code), request.urlPath);
		try {
			readFileBinary(response.errorPageByCode, response.body);
			response.contentLenght = response.body.length();
			response.contentType = getContentType(response.errorPageByCode);
		} catch (std::exception &e) {
			std::cerr << "Caught exception: " << e.what() << std::endl;
			std::cerr << "was unable to read error page: " << response.errorPageByCode << std::endl;
			fallback_error(response, request.code);
			request.code = 500;
		}
	} else {
		response.errorPageByCode = "";
		response.body = body;
		if (request.autoindex) {
			response.contentType = "text/html";
		} else {
			response.contentType = getContentType(request.physicalPathCgi);
		}
		response.contentLenght = response.body.length();
	}
	response.method = request.method;
	codes(request.code, response.codeMessage);
	return response;
}

std::string	methods(parsRequest &request, std::map<std::string, std::vector<Server> > &servers, std::string &port, std::string &host, bool &CGI){
	
	response response;
	std::string body;
	
	std::string replyString;
	std::string hostPort = host + ":" + port;
	
	request = parseRequest(request.parsBuff, servers, hostPort);
	if (request.autoindex == true){
		if (request.code == 200){
			response = responseStructConstruct(servers, hostPort, request.requestBody, request);
		} else {
			response = responseStructConstruct(servers, hostPort, "", request);
		}
		replyString = formResponseString(response);
		return replyString;
	}
	
	if (request.code != 200){
		response = responseStructConstruct(servers, hostPort, "", request);
	} else {
		
		if (request.callCGI == true){
			CGI = true;
			request.callCGI = true;
			return "";
		} else if (request.method == GET){
			try {
				methodGet(request, body);
			} catch (std::exception &e) {
				std::cerr << "Caught exception: " << e.what() << std::endl;
				request.code = 500;
			}
			response = responseStructConstruct(servers, hostPort, body, request);
		} else if (request.method == DELETE){
			try {
				methodDelete(request);
			} catch (std::exception &e) {
				std::cerr << "Caught exception: " << e.what() << std::endl;
				request.code = 500;
			}
			response = responseStructConstruct(servers, hostPort, "", request);
		}
	}
	//reply
	if (request.code / 100 == 3){
		replyString = response.header;
	} else {
		replyString = formResponseString(response);
	}
	return replyString;
}
