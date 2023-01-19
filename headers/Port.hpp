//
//  Port.hpp
//  server_xcode_project
//
//  Created by Julia Demura on 11/01/2023.
//

#ifndef Port_hpp
#define Port_hpp

#include <stdio.h>
#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

class Port{
	
protected:
	struct addrinfo *_res;
	struct sockaddr_in _simp_res;
	
public:
	Port(void *inp);
	~Port();
	addrinfo *	getRes();
	sockaddr_in getSimplRes();
};

#endif /* Port_hpp */
