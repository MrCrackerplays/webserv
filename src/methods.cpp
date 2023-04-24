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
	
	//request.physicalPathCgi = "/Users/yuliia/Codam/webserv/root/upload_test.html";
	if (isDirectory(request.physicalPathCgi) == false && isFile(request.physicalPathCgi) == false){
		request.code = 404;
		return;
//	} else if (ifFileExecutable(request.physicalPathCgi) == false) {
//		request.code = 401;
//		return;
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
	std::string setCookie = getServer(servers, hostPort, request.hostNameHeader).getClosestLocation(request.urlPath).getSetCookie();
	if (setCookie != ""){
		response.setCookie = setCookie;
	}
	if (request.code % 100 == 3){ //redirection
		redirectionResponse(request.code, request.newlocation, response);
		return response;
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

void	ifCGItrue(std::string &cgiReply, parsRequest &request, std::string &port, std::string &host, std::string &hostPort, response &response, std::string &replyString, int &statusChild, std::map<std::string, std::vector<Server> > &servers){

	try {
		cgiReply = spawnProcess(request, port, host, statusChild, std::string("SAVE_LOCATION=") + getServer(servers, hostPort, request.hostNameHeader).getClosestLocation(request.urlPath).getSaveLocation());
	} catch (std::exception &e) {
		
		std::cerr << "Caught exception: " << e.what() << std::endl;
		request.code = 500;
		response = responseStructConstruct(servers, hostPort, "", request);
		replyString = formResponseString(response);
		return ;
	}

			
	if (statusChild < 0){

		std::cerr << "error in child proper error is still needed lol" <<std::endl; //UNFINISHED
		request.code = 500;
		response = responseStructConstruct(servers, hostPort, "", request);
	} else {

		response.method = request.method;
		parseCorrectResponseCGI(cgiReply, response);
	}
}


//in case error pages are going bad do we need to have default one to present?
std::string	methods(std::string parsBuff, std::map<std::string, std::vector<Server> > &servers, std::string &port, std::string &host){
	
	parsRequest request;
	response response;
	std::string body;
	
	std::string cgiReply;
	std::string replyString;
	int statusChild;
	std::string hostPort = host + ":" + port;

	// std::cout << "------------------------------" << std::endl;
	// std::cout << "buffer after request received: " << std::endl;
	// std::cout << parsBuff << std::endl;
	// std::cout << "------------------------------" << std::endl;
	
	request = parseRequest(parsBuff, servers, hostPort);
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
		//std::cout << "method check1------" << std::endl;
		response = responseStructConstruct(servers, hostPort, "", request);
	} else {
		
		if (request.callCGI == true){
			//CGI - block needs to be moved to separate function in separate file
			//i need to return from method if ther is a cgi request
			//so in event loop I can create pipes -> vCGI, it will go through poll
			//I need separate write part for CGI and read part for CGI in event loop
			ifCGItrue(cgiReply, request, port, host, hostPort, response, replyString, statusChild, servers);
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
	//std::cout << "METOD FINISHED" << std::endl;
	return replyString;
}
