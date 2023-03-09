//
//  constructResponse.cpp
//  server_xcode_project
//
//  Created by Julia Demura on 15/02/2023.
//

#include "constructResponse.hpp"

std::string constructResponseHeader(unsigned long contentLength, int codeStatus, std::string& codeMessage) {

	std::string responseHeader = "HTTP/1.1 " + codeMessage + "\r\n"; ///in later codes version the codeStatus is already in a code message string
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
	std::string codeMessage;
	
	codes(codeStatus, codeMessage, body);
	
	switch (type) {
		case NOTSPECIFERR: //error in request
			return 0;
		
		case GET:
			body = constructResponseBody(filename);
			head = constructResponseHeader(body.length(), codeStatus, codeMessage);
			return head + body;
		
		case POST:
			body = constructResponseBody(filename);
			head = constructResponseHeader(body.length(), codeStatus, codeMessage);
			return head + body;
		
		case DELETE:
			return response = constructResponseHeader(0, codeStatus, codeMessage);
			
	}
	return 0;
}
