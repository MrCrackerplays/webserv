//
//  methods.cpp
//  server_xcode_project
//
//  Created by Julia Demura on 16/02/2023.
//

#include "methods.hpp"
#include "constructResponse.hpp"
#include <sstream>
//https://cplusplus.com/reference/sstream/stringstream/

//In a GET request, the client asks the server to retrieve a resource identified by a specific URL. The server responds with the resource in the response body, along with some metadata in the headers.

//std::string readFile(std::string &path){
//
//	std::fstream fileContent(path);
//	//maybe checks needed here?
//	std::stringstream streamFile;
//	streamFile << fileContent.rdbuf();
//
//	return streamFile.str();
//}


void	methods(parsRequest parsRes){

	//get should +- work
	
	formResponseString(parsRes.method, parsRes.status, parsRes.urlPath);
	
}

