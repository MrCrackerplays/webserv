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
	if (response.setCookie != "")
		response.header += "Set-Cookie: " + response.setCookie + "\r\n";
	response.header  += "\r\n";
	response.body = "";
}

void	parseCorrectResponseCGI(std::string& CGIbuff, response& response){


	//std::cout << "parseCorrectResponseCGI" << std::endl;
	//std::cout << CGIbuff << std::endl;
	std::string contentHeader = "Content-Type:";
	size_t pos = CGIbuff.find("\r\n\r\n"); //end of header
	if (pos == std::string::npos){
		//std::cout << "pos == npos" << std::endl;
		response.body = CGIbuff;
		response.contentLenght = CGIbuff.length();
		//std::cout << "codes bfr" << std::endl;
		codes(200, response.codeMessage);
		//std::cout << "parseCorrectResponseCGI - end" << std::endl;
		return;
	}
	//std::cout << "pos != npos" << std::endl;
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
	//std::cout << "parseCorrectResponseCGI - end" << std::endl;
}

void constructResponseHeader(response& response) {

	//with redirect no cont len or type, but another parts
	response.header = "HTTP/1.1 " + response.codeMessage + "\r\n";
	response.header  += "Content-Type: " + response.contentType + "\r\n";
	response.header += "Content-Length: " + std::to_string(response.contentLenght) + "\r\n";
	if (response.setCookie != "")
		response.header += "Set-Cookie: " + response.setCookie + "\r\n";
	response.header  += "\r\n";
	//std::cout << "constructResponseHeader end" << std::endl;
}

std::string	formResponseString(response response){
	
	//std::cout << "formResponseString" << std::endl;
	//std::cout << response.method << std::endl;
	constructResponseHeader(response);
	
	switch (response.method) {
		case ERR:
			//std::cout << "formResponseString: ERR" << std::endl;
			return response.header + response.errorPageByCode;
		case NOTSUPPORTED:
			//std::cout << "formResponseString: NSPRT" << std::endl;
			return response.header + response.errorPageByCode;
		
		case GET:
			//std::cout << "formResponseString: GET" << std::endl;
			return response.header + response.body;
			
		case DELETE:
			//std::cout << "formResponseString: DEL" << std::endl;
			return response.header;
			
		case POST:
			//std::cout << "formResponseString: POST" << std::endl;
			return response.header + response.body; //UNFINISHED
	}
	//std::cout << "formResponseString should not be here" << std::endl;
	return 0;
}
