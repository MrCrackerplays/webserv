//
//  constructResponse.cpp
//  server_xcode_project
//
//  Created by Julia Demura on 15/02/2023.
//

#include "constructResponse.hpp"
#include "codes.hpp"

void	redirectionResponse(int code, std::string newlocation, response &response){
	codes(code, response.codeMessage);
	response.header = "HTTP/1.1 " + response.codeMessage + "\r\n";
	response.header  += "Location: " + newlocation + "\r\n";
	response.header += "Content-Length: " + std::to_string(0) + "\r\n";
	response.header  += "\r\n";
	response.body = "";
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

void constructResponseHeader(response& response) {

	//with redirect no cont len or type, but another parts
	response.header = "HTTP/1.1 " + response.codeMessage + "\r\n";
	response.header  += "Content-Type: " + response.contentType + "\r\n";
	response.header += "Content-Length: " + std::to_string(response.contentLenght) + "\r\n";
	response.header  += "\r\n";
}

std::string	formResponseString(response response){
	
	constructResponseHeader(response);
	
	switch (response.method) {
		case ERR:
			return response.header + response.errorPageByCode;
		case NOTSUPPORTED:
			return response.header + response.errorPageByCode;
		
		case GET:
			return response.header + response.body;
			
		case DELETE:
			return response.header;
			
		case POST:
			return response.header + response.body; //UNFINISHED
	}
	return 0;
}
