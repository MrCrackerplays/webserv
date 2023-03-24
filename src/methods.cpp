//
//  methods.cpp
//  server_xcode_project
//
//  Created by Julia Demura on 16/02/2023.
//

#include "methods.hpp"
#include "constructResponse.hpp"
#include "parseRequest.hpp"
#include "codes.hpp"
#include "spawnProcess.hpp"
#include <cstdio>
#include <map>


//existance, read and executable checks
//streaming binary file content in body
void	methodGet(parsRequest& request, std::string& body){

	if (isDirectory(request.physicalPathCgi) == false && isFile(request.physicalPathCgi) == false){
		request.code = NOTFOUND;
	} else if (ifFileExecutable(request.physicalPathCgi) == false) {
		request.code = FORBIDDEN;
	} else if (ifFileReadable(request.physicalPathCgi) == false) {
		request.code = FORBIDDEN;
	}
	//if everything is OK with the file:
	std::ifstream file(request.physicalPathCgi, std::ios::binary);
	if (!file.is_open()) {
		throw std::runtime_error("methods : open");
	}
	std::string responseBody((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	body = responseBody;
}

//existance, write checks
//removing file with physical path provided
void methodDelete(parsRequest& request){

	if (isDirectory(request.physicalPathCgi) == false && isFile(request.physicalPathCgi) == false){
		request.code = NOTFOUND;
	} else if (ifFileWritable(request.physicalPathCgi) == false){
		request.code = FORBIDDEN;
	}
	//if everything is OK with the file:
	if (std::remove(request.physicalPathCgi.c_str()) != 0){
		throw std::runtime_error("methods : remove");
	}
	
}

Server & getServer(std::map<std::string, std::vector<Server> > &servers, std::string& hostPort, std::string& hostNameHeader){
	
	std::vector<Server> serversVect = servers[hostPort];
	std::vector<Server>::iterator it;
	if (hostNameHeader.length()){
		for (it = serversVect.begin(); it < serversVect.end(); it++) {
			Server & local = *it;
			for (std::vector<const std::string>::iterator it2 = local.getNames().begin(); it2 < local.getNames().end(); it2++){
				std::string str = *it2;
				if (str == hostNameHeader){
					return local;
				}
			}
		}
	}
	return *it;
}

//https://www.geeksforgeeks.org/http-headers-content-type/
std::string getContentType(std::string& filename){
	
	std::map<std::string, std::string> extensionsMap;
	extensionsMap.insert(std::make_pair("html", "text/html"));
	extensionsMap.insert(std::make_pair("htm", "text/html"));
	extensionsMap.insert(std::make_pair("txt", "text/plain"));
	extensionsMap.insert(std::make_pair("jpg", "image/jpeg"));
	extensionsMap.insert(std::make_pair("jpeg", "image/jpeg"));
	extensionsMap.insert(std::make_pair("png", "image/png"));
	extensionsMap.insert(std::make_pair("gif", "image/gif"));
	extensionsMap.insert(std::make_pair("pdf", "application/pdf"));

	std::string contentType;
	size_t pos = filename.rfind(".");
	if (pos != std::string::npos){
		std::string extension = filename.substr(pos + 1);
		std::map<std::string, std::string>::iterator it = extensionsMap.find(extension);
		if (it != extensionsMap.end()){
			return it->second;
		}
	}
	return "text/plain"; //default
}

response responseStructConstruct(std::map<std::string, std::vector<Server> > &servers, std::string& hostPort, std::string body, parsRequest& request){
	
	response res;
	if (request.code != OK){
		res.body = "";
		//any default error page?
		res.errorPageByCode = getServer(servers, hostPort, request.hostNameHeader).getErrorPage(std::to_string(request.code), request.urlPath);
		res.contentLenght = res.errorPageByCode.length();
		res.contentType = getContentType(res.errorPageByCode);
	} else {
		res.errorPageByCode = "";
		res.body = body;
		res.contentType = getContentType(request.physicalPathCgi);
		res.contentLenght = res.body.length();
	}
	
	res.method = request.method;
	codes(request.code, res.codeMessage);
	
	return res;
}

//bool	ifCallCGI(parsRequest& request, std::map<std::string, std::vector<Server> > &servers, std::string& hostPort){
//
//	///after I get cgi from request i can compare if this one will work for us
//	///if the cgi I want to see closest location methods list and compare
//	Location location = getServer(servers, hostPort, request.hostNameHeader).getClosestLocation(request.urlPath);
//	std::vector<std::string> methods = location.getMethods();
//	///i compare the extention in getCGI in location std::find in vector
//	if (request.method == POST){
//		return true;
//	} else if (request.method == GET) {
//		///and if cgi is in config - check with Patrick how to compare it
//		return true;
//	} else if (request.method == DELETE) {
//		///and if cgi is in config - check with Patrick how to compare it
//		return true;
//	}
//	return false;
//}

void	parseCorrectResponseCGI(std::string& CGIbuff, response& response){
	
	std::string contentHeader = "Content-Type:";
	size_t pos = CGIbuff.find("\r\n\r\n"); //end of header
	
	//header
	std::string headerAfterCgi = CGIbuff.substr(0, pos);
	size_t start = headerAfterCgi.find(contentHeader);
	size_t end = headerAfterCgi.find("\r\n");
	if (start != end){
		response.contentType = headerAfterCgi.substr(start + contentHeader.length(), end);
	} else {
		response.contentType = "";
	}
	//body
	response.body = CGIbuff.substr(pos+4, CGIbuff.length());
	response.contentLenght = response.body.length();
	codes(OK, response.codeMessage);
}

void	methods(std::string parsBuff, std::map<std::string, std::vector<Server> > &servers, std::string port, std::string host){
	
	parsRequest request;
	
	std::string cgiReply;
	int statusChild;
	
	request = parseRequest(parsBuff, servers, port, host);
	if (request.code != OK){
		//error generator
	}
	std::string hostPort = host + ":" + port;
	
	if (request.callCGI == true){
		cgiReply = spawnProcess(request, port, host, statusChild);
		if (statusChild < 0){
			//error in child
			//generate error response
		}
	}
	
	
	
	//if not CGI, methods :
	if (request.method == GET){
		std::string body;
		methodGet(request, body);
		response response = responseStructConstruct(servers, hostPort, body, request);
	} else if (request.method == DELETE){
		
		
	}
	
	//end of the process
	//formResponseString(parsRes.method, parsRes.status, parsRes.urlPath); //unfinished
}

