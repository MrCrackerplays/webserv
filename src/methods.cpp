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

response responseStructConstruct(std::map<std::string, std::vector<Server> > &servers, std::string& hostPort, std::string body, parsRequest& request){
	
	response response;
	if (request.code != OK){
		response.body = "";
		response.errorPageByCode = getServer(servers, hostPort, request.hostNameHeader).getErrorPage(std::to_string(request.code), request.urlPath); //any default error page?
		response.body = response.errorPageByCode;
		response.contentLenght = response.errorPageByCode.length();
		response.contentType = getContentType(response.errorPageByCode);
	} else {
		response.errorPageByCode = "";
		response.body = body;
		response.contentType = getContentType(request.physicalPathCgi);
		response.contentLenght = response.body.length();
	}
	
	response.method = request.method;
	codes(request.code, response.codeMessage);
	
	return response;
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
		response.contentType = "text/plain";
	}
	//body
	response.body = CGIbuff.substr(pos+4, CGIbuff.length());
	response.contentLenght = response.body.length();
	codes(OK, response.codeMessage);
}

void	methods(std::string parsBuff, std::map<std::string, std::vector<Server> > &servers, std::string port, std::string host){
	
	parsRequest request;
	response response;
	
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
	} else if (request.method == GET){
		std::string body;
		methodGet(request, body);
		response = responseStructConstruct(servers, hostPort, body, request);
	} else if (request.method == DELETE){
		methodDelete(request);
		response = responseStructConstruct(servers, hostPort, "", request);
	}
	
	//end of the process
	//formResponseString(parsRes.method, parsRes.status, parsRes.urlPath); //unfinished
}

