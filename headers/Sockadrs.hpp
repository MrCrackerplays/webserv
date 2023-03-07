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
	//struct sockaddr_in _simp_res;
	
public:
	Sockadrs(char * hostName, char * portNumber);
	~Sockadrs();
	addrinfo *	getRes();
	//sockaddr_in getSimplRes();
};

#endif /* Sockadrs_hpp */
