//
//  Sockadrs.hpp
//  server_xcode_project
//
//  Created by Julia Demura on 11/01/2023.
//

#ifndef Sockadrs_hpp
#define Sockadrs_hpp

#include <stdio.h>
#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

class Sockadrs{
	
protected:
	struct addrinfo *_res;
	std::string _hostName;
	std::string _portNumber;
	//struct sockaddr_in _simp_res;
	
public:
	Sockadrs(char * hostName, char * portNumber);
	~Sockadrs();
	addrinfo *	getRes();
	std::string getHostName();
	std::string getPortNumber();
	//sockaddr_in getSimplRes();
};

#endif /* Sockadrs_hpp */
