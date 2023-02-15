//
//  constructResponse.cpp
//  server_xcode_project
//
//  Created by Julia Demura on 15/02/2023.
//

#include "constructResponse.hpp"
#include <string>
#include <fstream>
#include <iostream>
#include <sys/socket.h>

std::string pickCodeMessage(int codeStatus){
	
	std::string codeMessage;
	switch (codeStatus) {
		case 200:
			codeMessage = " OK";
		case 400:
			codeMessage = " Bad Request";
		case 404:
			codeMessage = " Not Found";
	}
	
	return codeMessage;
}

std::string constructResponseHeader(unsigned long contentLength, int codeStatus) {
	
	std::string codeMessage = pickCodeMessage(codeStatus);
	
	std::string responseHeader = "HTTP/1.0 " + std::to_string(codeStatus) + codeMessage + "\r\n";
	responseHeader += "Content-Type: text/html\r\n";
	responseHeader += "Content-Length: " + std::to_string(contentLength) + "\r\n";
	responseHeader += "\r\n";
	return responseHeader;
}

std::string constructResponseBody(std::string filename) {
	
	std::ifstream file(filename);
	
	std::string responseBody((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	return responseBody;
}

std::string	formResponseString(request type, int codeStatus, std::string filename){
	
	std::string response;
	std::string responseBody;
	
	switch (type) {
		case GET:
			response = constructResponseHeader(0, codeStatus);
		case POST:
			responseBody = constructResponseBody(filename);
			response = constructResponseHeader(responseBody.length(), codeStatus);
		case DELETE:
			response = constructResponseHeader(0, codeStatus);
	}
	return response;
}
