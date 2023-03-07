//
//  constructResponse.cpp
//  server_xcode_project
//
//  Created by Julia Demura on 15/02/2023.
//

#include "constructResponse.hpp"

std::string pickCodeMessage(int codeStat){
	
	std::string codeMessage;
	switch (codeStat) {
		case 100:
			return codeMessage = "";
		case 200:
			return codeMessage = " OK";
		case 201:
			return codeMessage = " Created";
		case 204:
			return codeMessage = " No Content";
		case 301:
			return codeMessage = " Moved Permanently";
		case 302:
			return codeMessage = " Found";
		case 304:
			return codeMessage = " Not Modified";
		case 400:
			return codeMessage = " Bad Request";
		case 401:
			return codeMessage = " Unauthorized";
		case 404:
			return codeMessage = " Not Found";
	}
	return 0;
}

std::string constructResponseHeader(unsigned long contentLength, int codeStatus) {
	
	std::string codeMessage = pickCodeMessage(codeStatus);
	
	std::string responseHeader = "HTTP/1.1 " + std::to_string(codeStatus) + codeMessage + "\r\n";
	responseHeader += "Content-Type: text/html\r\n";
	responseHeader += "Content-Length: " + std::to_string(contentLength) + "\r\n";
	responseHeader += "\r\n";
	return responseHeader;
}

std::string constructResponseBody(std::string& filename) {
	
	std::ifstream file(filename);
	
	std::string responseBody((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	return responseBody;
}

std::string	formResponseString(method type, int codeStatus, std::string& filename){
	
	std::string response;
	std::string body;
	std::string head;
	
	switch (type) {
		case NOTSPECIFERR: //error in request
			return 0;
		
		
		case GET:
			body = constructResponseBody(filename);
			head = constructResponseHeader(body.length(), codeStatus);
			return head + body;
		
		
		case POST:
			body = constructResponseBody(filename);
			head = constructResponseHeader(body.length(), codeStatus);
			return head + body;
			
		
		case DELETE:
			return response = constructResponseHeader(0, codeStatus);
			
	}
	return 0;
}
