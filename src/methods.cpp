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
//CHECK if we are handling that (dont thinks so)
	//file can be in server root
	if (isDirectory(request.physicalPathCgi) == false && isFile(request.physicalPathCgi) == false){
		request.code = 404;
		return;
	} else if (ifFileExecutable(request.physicalPathCgi) == false) {
		request.code = 401;
		return;
	} else if (ifFileReadable(request.physicalPathCgi) == false) {
		request.code = 401;
		return;
	}
	readFileBinary(request.physicalPathCgi, body);
}

//existance, write checks
//removing file with physical path provided
void methodDelete(parsRequest& request){

	if (isDirectory(request.physicalPathCgi) == false && isFile(request.physicalPathCgi) == false){
		request.code = 404;
	} else if (ifFileWritable(request.physicalPathCgi) == false){
		request.code = 401;
	}
	if (std::remove(request.physicalPathCgi.c_str()) != 0){
		throw std::runtime_error("methods : remove");
	}
	
}

response responseStructConstruct(std::map<std::string, std::vector<Server> > &servers, std::string& hostPort, std::string body, parsRequest& request){
	
	response response;
	if (request.code != 200){
		response.body = "";
		response.errorPageByCode = getServer(servers, hostPort, request.hostNameHeader).getErrorPage(std::to_string(request.code), request.urlPath);
		try {
			readFileBinary(response.errorPageByCode, response.body);
			response.contentLenght = response.errorPageByCode.length();
			response.contentType = getContentType(response.errorPageByCode);
		} catch (std::exception &e) {
			std::cerr << "Caught exception: " << e.what() << std::endl;
			request.code = 500;
		}
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
	codes(200, response.codeMessage);
}


//in case error pages are going bad do we need to have default one to present?
std::string	methods(std::string parsBuff, std::map<std::string, std::vector<Server> > &servers, std::string port, std::string host){
	
	parsRequest request;
	response response;
	
	std::string cgiReply;
	std::string replyString;
	int statusChild;
	std::string hostPort = host + ":" + port;
	
	request = parseRequest(parsBuff, servers, hostPort);
	if (request.code != 200){
		response = responseStructConstruct(servers, hostPort, "", request);
	} else {
		
		if (request.callCGI == true){
			try {
				cgiReply = spawnProcess(request, port, host, statusChild);
			} catch (std::exception &e) {
				std::cerr << "Caught exception: " << e.what() << std::endl;
				request.code = 500;
				response = responseStructConstruct(servers, hostPort, "", request);
				
			}
			if (statusChild < 0){
				std::cerr << "error in child proper error is still needed lol" <<std::endl;
				request.code = 500;
				response = responseStructConstruct(servers, hostPort, "", request);
			} else {
				parseCorrectResponseCGI(cgiReply, response);
			}
			
		} else if (request.method == GET){
			std::string body;
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
	
	
	//end of the process
		//unfinished?
	replyString = formResponseString(response);
	
	return replyString;
}

//try {
//
//} catch (std::exception &e) {
//	std::cerr << "Caught exception: " << e.what() << std::endl;
//	request.code = 500;
//}
