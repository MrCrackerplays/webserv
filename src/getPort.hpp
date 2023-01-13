//
//  getPort.hpp
//  server_xcode_project
//
//  Created by Julia Demura on 11/01/2023.
//

#ifndef getPort_hpp
#define getPort_hpp

#include <stdio.h>
#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>


class Port{
	
private:
//	std::string _port;
	struct addrinfo *_res; //is linked list
	struct addrinfo _hints;
	
	
public:
	Port();
	~Port();
//	void	getPort();
//	void	getAddrss();
	void	getRes();
	void	getHints();
	
	
	
	
};

#endif /* getPort_hpp */
