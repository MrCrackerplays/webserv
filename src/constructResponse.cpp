//
//  constructResponse.cpp
//  server_xcode_project
//
//  Created by Julia Demura on 15/02/2023.
//

#include "constructResponse.hpp"

void constructResponseHeader(response& response) {

	response.header = "HTTP/1.1 " + response.codeMessage + "\r\n";
	response.header  += "Content-Type: " + response.contentType + "\r\n";
	response.header += "Content-Length: " + std::to_string(response.contentLenght) + "\r\n";
	response.header  += "\r\n";
}

std::string	formResponseString(response response, std::string& filename){

	constructResponseHeader(response);
	
	switch (response.method) {
		case ERR:
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
