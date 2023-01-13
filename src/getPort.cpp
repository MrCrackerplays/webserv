//
//  getPort.cpp
//  server_xcode_project
//
//  Created by Julia Demura on 11/01/2023.
//

#include "getPort.hpp"
//getaddrinfo, freeaddrinfo, gai_strerror - network address and service translation
//https://cppsecrets.com/users/18989711511997116104103495564103109971051084699111109/C00-Socket-Programming-getaddrinfo.php
//https://man7.org/linux/man-pages/man3/getaddrinfo.3.html

//The addrinfo structure used by getaddrinfo() contains the
//	   following fields:
//
//		   struct addrinfo {
//			   int              ai_flags;
//			   int              ai_family;
//			   int              ai_socktype;
//			   int              ai_protocol;
//			   socklen_t        ai_addrlen;
//			   struct sockaddr *ai_addr;
//			   char            *ai_canonname;
//			   struct addrinfo *ai_next;
//		   };



//	   const char *gai_strerror(int errcode);


///This code is a simple example of how to use the getaddrinfo() function in C++ to set up the address and port for a server. The getaddrinfo() function is used to translate hostnames and service names into a socket address structure, which can be used to create a socket for the server.
///The code does the following:
///Declares two variables hints of type struct addrinfo and res which is a pointer to struct addrinfo.
///Initialize the hints structure by setting all its bytes to zero using memset() function.
///Specify the configuration of the socket:
///hints.ai_family to AF_UNSPEC tells that it doesn't care if it is an IPv4 or IPv6 address.
///hints.ai_socktype to SOCK_STREAM is used to specify that it will be using TCP stream sockets
///hints.ai_flags to AI_PASSIVE is used to fill in the IP address of the server for the function getaddrinfo() to bind the socket to.
///It calls the getaddrinfo() function, passing it NULL as the first argument to specify that the server should listen on all available IP addresses, "3490" as the second argument to specify that it should listen on port 3490, and passing it a pointer to the hints structure, and a pointer to the res variable.
///Check if getaddrinfo() returned an error or not by checking the return value in the status variable.
///If getaddrinfo() succeeded, it returns a pointer to a linked list of struct addrinfo in the res variable.
///Once the code is done using the res variable, it frees the memory used by the linked list using the freeaddrinfo() function.
///The code then returns 0 to indicate that it has completed successfully.
Port::Port(){
	
	int status;
	char* port_number;
	
	std::memset(&_hints, 0, sizeof(_hints));
	_hints.ai_family = AF_UNSPEC; // don't care IPv4 or IPv6
	_hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
	_hints.ai_flags = AI_PASSIVE; //fill ip address by default
	
	
	status = getaddrinfo(NULL, port_number, &_hints, &_res);
	
	
	
	
}

Port::~Port(){
	//void freeaddrinfo(struct addrinfo *res);
	
}

//void	Port::getPort(){
//
//}



