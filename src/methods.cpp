//
//  methods.cpp
//  server_xcode_project
//
//  Created by Julia Demura on 16/02/2023.
//

#include "methods.hpp"
#include "constructResponse.hpp"
#include "parseRequest.hpp"
#include "codes.hpp"
#include "spawnProcess.hpp"

//usually request does not have the body. CHECH CGI
//usually response have a body. with error or with answer
//find file in server.
//check access to file
//get the correct file or the error html in the body. - partially done in response

void methodGet(parsRequest& request){

	if (isDirectory(request.physicalPathCgi) == false && isFile(request.physicalPathCgi) == false){
		request.status = NOTFOUND;
	} else if (ifFileExecutable(request.physicalPathCgi) == false){
		request.status = FORBIDDEN;
	}
	
	
	//eventually if everything is OK with the file:
	request.body = request.physicalPathCgi;
}

response initiateResponseStruct(parsRequest &request){
	
	response res;
	
	res.method = request.method;
	codes(request.status, res.codeMessage, res.body);
	//get content type by checking server?
	//get error body-page by code
	
	return res;
}


void	methods(std::string parsBuff, std::map<std::string, std::vector<Server> > &servers, std::string port, std::string host){
	
	parsRequest parsRes;
	response response;
	parsRes = parseRequest(parsBuff, servers, port, host);
	
	
	if (parsRes.method == POST){ //or if coinfig have CGI, then any method
		response.cgiReply = spawnProcess(parsRes, port, host);
	}
	
	
	
	//check if i have cgi match with method and if I need execve cgi reference is vector of str
	
	//after I get cgi from request i can compare if this one will work for us
	//if the cgi I want to see closest location methods list and compare
	
	//if not CGI, methods :
	//end of the process
	//formResponseString(parsRes.method, parsRes.status, parsRes.urlPath);
	
}

