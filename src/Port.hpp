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
	
private:
	struct addrinfo *_res; //is linked list and final result
	struct addrinfo _hints; //not sure if needed
	
public:
	Port();
	~Port();
	addrinfo *	getRes();
	addrinfo	getHints();
};

#endif /* Port_hpp */
