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

static void	fallback_error(response& response) {
	response.body = "<html><head><title>Internal Server Error</title><style>*{text-align: center;}</style></head><body><h1>An Internal Server Error Has Occurred</h1><hr>Webserv</body></html>";
	response.contentLenght = response.body.length();
	response.contentType = "text/html";
}

response responseStructConstruct(std::map<std::string, std::vector<Server> > &servers, std::string& hostPort, std::string body, parsRequest& request){
	
	response response;
	std::string setCookie = getServer(servers, hostPort, request.hostNameHeader).getClosestLocation(request.urlPath).getSetCookie();
	if (setCookie != ""){
		response.setCookie = setCookie;
	}
	if (request.code / 100 == 3){ //redirection
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
			std::cerr << "was unable to read error page: " << response.errorPageByCode << std::endl;
			fallback_error(response);
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


//in case error pages are going bad do we need to have default one to present?
std::string	methods(std::string parsBuff, std::map<std::string, std::vector<Server> > &servers, std::string port, std::string host){
	
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
		
		//CGI
		if (request.callCGI == true){
			try {
				cgiReply = spawnProcess(request, port, host, statusChild, std::string("SAVE_LOCATION=") + getServer(servers, hostPort, request.hostNameHeader).getClosestLocation(request.urlPath).getSaveLocation());
			} catch (std::exception &e) {
				//std::cout << "method check2------" << std::endl;
				std::cerr << "Caught exception: " << e.what() << std::endl;
				request.code = 500;
				response = responseStructConstruct(servers, hostPort, "", request);
				replyString = formResponseString(response);
				return replyString;
			}
			
			std::cout << "methods: status Child:" << statusChild << std::endl;
			
			if (statusChild < 0){
				//std::cout << "method check3------" << std::endl;
				std::cerr << "error in child proper error is still needed lol" <<std::endl; //UNFINISHED
				request.code = 500;
				response = responseStructConstruct(servers, hostPort, "", request);
			} else {
				
				//std::cout << "method check4------" << std::endl;
				//std::cout << "++++++++++ method : " << request.methodString << std::endl;
				//std::cout << "++++++++++ method int: " << request.method << std::endl;
				response.method = request.method;
				parseCorrectResponseCGI(cgiReply, response);
			}
		// GET
		} else if (request.method == GET){
			try {
				methodGet(request, body);
			} catch (std::exception &e) {
				std::cerr << "Caught exception: " << e.what() << std::endl;
				request.code = 500;
			}
			response = responseStructConstruct(servers, hostPort, body, request);
		//DELETE
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
