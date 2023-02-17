//
//  parsingRequest.cpp
//  server_xcode_project
//
//  Created by Julia Demura on 16/02/2023.
//

#include "parsingRequest.hpp"
#include <map>
#include <iostream>
#include <sstream>
//https://cplusplus.com/reference/sstream/istringstream/


//GET /favicon.ico HTTP/1.1
//Host: localhost:8011
//Connection: keep-alive
//sec-ch-ua: "Not_A Brand";v="99", "Google Chrome";v="109", "Chromium";v="109"
//sec-ch-ua-mobile: ?0
//User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/109.0.0.0 Safari/537.36
//sec-ch-ua-platform: "macOS"
//Accept: image/avif,image/webp,image/apng,image/svg+xml,image/*,*/*;q=0.8
//Sec-Fetch-Site: same-origin
//Sec-Fetch-Mode: no-cors
//Sec-Fetch-Dest: image
//Referer: http://localhost:8011/
//Accept-Encoding: gzip, deflate, br
//Accept-Language: en-US,en;q=0.9,uk;q=0.8,ru-RU;q=0.7,ru;q=0.6,nl;q=0.5
//
//oding: gzip, deflate, br
//Accept-Language: en-US,en;q=0.9,uk;q=0.8,ru-RU;q=0.7,ru;q=0.6,nl;q=0.5

request	getMethod(std::istringstream & requestStream){
	
	std::string method;
	requestStream >> method;
	
	if (method.compare("GET") == 0){
		return GET;
	} else if (method.compare("POST") == 0) {
		return POST;
	} else if (method.compare("DELETE") == 0) {
		return DELETE;
	}
	return NOTSPECIF;
}

void parsingRequest(char *parsBuff){
	
	parsRes pars;

	std::string request(parsBuff);
	std::istringstream requestStream(request);
	
	pars.status = OK;
	pars.method = getMethod(requestStream);
	requestStream >> pars.path >> pars.httpVers;
	
	
	//create map container to store headers
	
	
}

