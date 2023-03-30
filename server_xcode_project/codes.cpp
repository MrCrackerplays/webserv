//
//  codes.cpp
//  server_xcode_project
//
//  Created by Julia Demura on 06/03/2023.
//

#include "codes.hpp"
#include <map>
//https://www.restapitutorial.com/httpstatuscodes.html

void	codes(int codeStatus, std::string &codeMessage){
	
	std::map<int, std::string> codesMap;
	
// 1xx Informational
	codesMap.insert(std::make_pair(100, "100 Continue"));
	codesMap.insert(std::make_pair(101, "101 Switching Protocols"));
	codesMap.insert(std::make_pair(102, "102 Processing"));
	codesMap.insert(std::make_pair(103, "103 Early Hints"));

 // 2xx Successful
	codesMap.insert(std::make_pair(200, "200 OK"));
	codesMap.insert(std::make_pair(201, "201 Created"));
	codesMap.insert(std::make_pair(202, "202 Accepted"));
	codesMap.insert(std::make_pair(203, "203 Non-Authoritative Information"));
	codesMap.insert(std::make_pair(204, "204 No Content"));
	codesMap.insert(std::make_pair(205, "205 Reset Content"));
	codesMap.insert(std::make_pair(206, "206 Partial Content"));
	codesMap.insert(std::make_pair(207, "207 Multi-Status"));
	codesMap.insert(std::make_pair(208, "208 Already Reported"));
	codesMap.insert(std::make_pair(226, "226 IM Used"));
 
 // 3xx Redirection
	codesMap.insert(std::make_pair(300, "300 Multiple Choices"));
	codesMap.insert(std::make_pair(301, "301 Moved Permanently"));
	codesMap.insert(std::make_pair(302, "302 Found"));
	codesMap.insert(std::make_pair(303, "303 See Other"));
	codesMap.insert(std::make_pair(304, "304 Not Modified"));
	codesMap.insert(std::make_pair(305, "305 Use Proxy"));
	codesMap.insert(std::make_pair(306, "306 Switch Proxy"));
	codesMap.insert(std::make_pair(307, "307 Temporary Redirect"));
	codesMap.insert(std::make_pair(308, "308 Permanent Redirect"));
 
 // 4xx Client Error
	codesMap.insert(std::make_pair(400, "400 Bad Request"));
	codesMap.insert(std::make_pair(401, "401 Unauthorized"));
	codesMap.insert(std::make_pair(402, "402 Payment Required"));
	codesMap.insert(std::make_pair(403, "403 Forbidden"));
	codesMap.insert(std::make_pair(404, "404 Not Found"));
	codesMap.insert(std::make_pair(405, "405 Method Not Allowed"));
	codesMap.insert(std::make_pair(406, "406 Not Acceptable"));
	codesMap.insert(std::make_pair(407, "407 Proxy Authentication Required"));
	codesMap.insert(std::make_pair(408, "408 Request Timeout"));
	codesMap.insert(std::make_pair(409, "409 Conflict"));
	codesMap.insert(std::make_pair(410, "410 Gone"));
	codesMap.insert(std::make_pair(411, "411 Length Required"));
	codesMap.insert(std::make_pair(412, "412 Precondition Failed"));
	codesMap.insert(std::make_pair(413, "413 Content Too Large"));
	codesMap.insert(std::make_pair(414, "414 URI Too Long"));
	codesMap.insert(std::make_pair(415, "415 Unsupported Media Type"));
	codesMap.insert(std::make_pair(416, "416 Range Not Satisfiable"));
	codesMap.insert(std::make_pair(417, "417 Expectation Failed"));
	
// 5xx Server Error
	codesMap.insert(std::make_pair(500, "500 Internal Server Error"));
	codesMap.insert(std::make_pair(501, "501 Not Implemented"));
	codesMap.insert(std::make_pair(502, "502 Bad Gateway"));
	codesMap.insert(std::make_pair(503, "503 Service Unavailable"));
	codesMap.insert(std::make_pair(504, "504 Gateway Timeout"));
	codesMap.insert(std::make_pair(505, "505 HTTP Version Not Supported"));
	
	std::map<int, std::string>::iterator it;
	it = codesMap.find(codeStatus);
	
	if (it != codesMap.end()){
		codeMessage = it->second;
	} else {
		
		switch (codeStatus / 100) {
			case 2:
				codeMessage = "2xx OK";
				break;
			case 3:
				codeMessage = "3xx OK";
				break;
			case 4:
				codeMessage = "4xx OK";
				break;
			case 5:
				codeMessage = "5xx OK";
				break;

		}
		
	}
}
