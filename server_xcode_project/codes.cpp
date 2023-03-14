//
//  codes.cpp
//  server_xcode_project
//
//  Created by Julia Demura on 06/03/2023.
//

#include "codes.hpp"
//https://www.restapitutorial.com/httpstatuscodes.html

void	three(int codeStatus, std::string &codeMessage){
	
	switch (codeStatus) {
//		case 300:
//			codeMessage = " ";
//			break;
		default:
			codeMessage = "300 Redirection";
			break;
	}
}

void	four(int codeStatus, std::string &codeMessage){
	
	switch (codeStatus) {
		case 400:
			codeMessage = "400 Bad Request";
			break;
		case 401:
			codeMessage = "401 Unauthorized";
			break;
		case 403:
			codeMessage = "403 Forbidden";
			break;
		case 404:
			codeMessage = "404 Not Found";
			break;
		default:
			codeMessage = "400 Client Error";
			break;
	}
}

void	five(int codeStatus, std::string &codeMessage){
	
	switch (codeStatus) {
		case 500:
			codeMessage = "500 Internal Server Error";
			break;
		default:
			codeMessage = "500 Server Error";
			break;
	}
}

void	codes(int codeStatus, std::string &codeMessage){

	switch (codeStatus / 100) {
		case 2:
			codeMessage = "200 OK";
			break;
		case 3:
			three(codeStatus, codeMessage);
			break;
		case 4:
			four(codeStatus, codeMessage);
			break;
		case 5:
			five(codeStatus, codeMessage);
			break;

	}

}
