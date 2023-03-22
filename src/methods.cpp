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

std::string methodGet(parsRequest& request){

	std::string body;
	if (isDirectory(request.physicalPathCgi) == false && isFile(request.physicalPathCgi) == false){
		request.code = NOTFOUND;
	} else if (ifFileExecutable(request.physicalPathCgi) == false){
		request.code = FORBIDDEN;
	}
	//eventually if everything is OK with the file:
	return body = request.physicalPathCgi;
}

void methodDelete(parsRequest& request){

	if (isDirectory(request.physicalPathCgi) == false && isFile(request.physicalPathCgi) == false){
		request.code = NOTFOUND;
	} else if (ifFileExecutable(request.physicalPathCgi) == false){
		request.code = FORBIDDEN;
	}
	//eventually if everything is OK with the file:
	//HOW to delete file
	
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
std::string getContentType(std::string filename){
	
//	std::string contentType;
//	size_t pos = filename.rfind("."); //find LAST dot
//	if (pos != std::string::npos){
//		return ("text/" + filename.substr(pos + 1, filename.length())); //if not default there are many options/end of the file. check if I need that(in link)
//	} else {
//		return "text/plain"; //default
//	}
//or always text/plain?
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
		res.contentType = getContentType(res.body);
		//or
		//res.contentType = getContentType(request.physicalPathCgi);
		//??
		res.contentLenght = res.body.length();
	}
	
	res.method = request.method;
	codes(request.code, res.codeMessage);
	
	return res;
}

bool	ifCallCGI(parsRequest& request, std::map<std::string, std::vector<Server> > &servers, std::string& hostPort){
	
	
	//after I get cgi from request i can compare if this one will work for us
	//if the cgi I want to see closest location methods list and compare
	Location location = getServer(servers, hostPort, request.hostNameHeader).getClosestLocation(request.urlPath);
	std::vector<std::string> methods = location.getMethods();
	
	//i compare the extention in getCGI in location std::find in vector
	
	if (request.method == POST){
		return true;
	} else if (request.method == GET) {
		//and if cgi is in config - check with Patrick how to compare it
		return true;
	} else if (request.method == DELETE) {
		//and if cgi is in config - check with Patrick how to compare it
		return true;
	}
	return false;
}

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
	//check if i have cgi match with method and if I need execve cgi reference is vector of str
	if (ifCallCGI(request, servers, hostPort)){ //POST or if coinfig have CGI, then any method
		cgiReply = spawnProcess(request, port, host, statusChild);
		if (statusChild < 0){
			//error in child
			//generate error response
		}
	}
	//if not CGI, methods :
	if (request.method == GET){
		std::string body = methodGet(request);
		response response = responseStructConstruct(servers, hostPort, body, request);
	}
	//end of the process
	//formResponseString(parsRes.method, parsRes.status, parsRes.urlPath);
}

