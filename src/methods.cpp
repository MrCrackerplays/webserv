//
//  methods.cpp
//  server_xcode_project
//
//  Created by Julia Demura on 16/02/2023.
//

#include "methods.hpp"
#include "constructResponse.hpp"
#include <sstream>

void methodGet(parsRequest& request){
	
	//usually request does not have the body. CHECH CGI
	//usually response have a body. with error or with answer
	
	
	//find file in server.
	//check access to file
	//get the correct file or the error html in the body. - partially done in response
	
	if (isDirectory(request.physicalPathCgi) == false && isFile(request.physicalPathCgi) == false){
		
		//file is not file and not a directory
		request.status = NOTFOUND;
	} else if (ifFileExecutable(request.physicalPathCgi) == false){
		request.status = FORBIDDEN;
	}
	
	
}


void	methods(parsRequest parsRes){
	
	//check if i have cgi match with method and if I need execve cgi reference is vector of str
	//after I get cgi from request i can compare if this one will work for us
	//if the cgi I want to see closest location methods list and compare
	
	//if not CGI, methods :
	
	
	//end of the process
	//formResponseString(parsRes.method, parsRes.status, parsRes.urlPath);
	
}

