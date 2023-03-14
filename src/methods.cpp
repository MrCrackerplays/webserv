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

void methodGet(parsRequest& request){

	if (isDirectory(request.physicalPathCgi) == false && isFile(request.physicalPathCgi) == false){
		request.code = NOTFOUND;
	} else if (ifFileExecutable(request.physicalPathCgi) == false){
		request.code = FORBIDDEN;
	}

	//eventually if everything is OK with the file:
	request.body = request.physicalPathCgi;
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
	std::string contentType;
	size_t pos = filename.find(".");
	if (pos != std::string::npos){
		return ("text/" + filename.substr(pos + 1, filename.length())); //if not default there are many options/end of the file. check if I need that(in link)
	} else {
		return "text/plain"; //default
	}
//or always text/plain?
}

response responseStruct(std::map<std::string, std::vector<Server> > &servers, std::string& hostPort, std::string body, parsRequest& request){
	
	response res;
	
	res.method = request.method;
	codes(request.code, res.codeMessage);
	res.contentType = getContentType(request.physicalPathCgi);
	
	if (request.code != OK){
		res.body = "";
		res.errorPageByCode = getServer(servers, hostPort, request.hostNameHeader).getErrorPage(std::to_string(request.code), request.urlPath);
		
	} else {
		res.errorPageByCode = "";
		res.body = body;
	}
	return res;
}

bool	ifCallCGI(parsRequest& request, std::map<std::string, std::vector<Server> > &servers, std::string& hostPort){
	
	
	//after I get cgi from request i can compare if this one will work for us
	//if the cgi I want to see closest location methods list and compare
	Location location = getServer(servers, hostPort, request.hostNameHeader).getClosestLocation(request.urlPath);
	std::vector<std::string> methods = location.getMethods();
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

void	parseResponseCGI(std::string& CGIbuff){
	
	
}

void	methods(std::string parsBuff, std::map<std::string, std::vector<Server> > &servers, std::string port, std::string host){
	
	parsRequest request;
	response response;
	request = parseRequest(parsBuff, servers, port, host);
	if (request.code != OK){
		//error generator
	}
	
	std::string hostPort = host + ":" + port;
	//check if i have cgi match with method and if I need execve cgi reference is vector of str
	if (ifCallCGI(request, servers, hostPort)){ //POST or if coinfig have CGI, then any method
		response.cgiReply = spawnProcess(request, port, host);
	}

	
	//if not CGI, methods :
	
	
	//end of the process
	//formResponseString(parsRes.method, parsRes.status, parsRes.urlPath);
	
}

