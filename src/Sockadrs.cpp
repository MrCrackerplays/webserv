//
//  Sockadrs.cpp
//  server_xcode_project
//
//  Created by Julia Demura on 11/01/2023.
//

#include "Sockadrs.hpp"
#include <stdexcept>

//getaddrinfo, freeaddrinfo, gai_strerror - network address and service translation
//https://cppsecrets.com/users/18989711511997116104103495564103109971051084699111109/C00-Socket-Programming-getaddrinfo.php
//https://man7.org/linux/man-pages/man3/getaddrinfo.3.html

//The getaddrinfo() function is used to translate hostnames and service names into a socket address structure, which can be used to create a socket for the server.
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

//explanation of code:
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

//https://en.cppreference.com/w/cpp/error/runtime_error


Sockadrs::Sockadrs(char * hostName, char * portNumber){
	
	int status;
	struct addrinfo hints;
	
	std::memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
	hints.ai_flags = AI_PASSIVE; //fill ip address by default
	status = getaddrinfo(hostName, portNumber, &hints, &_res);
	if (status < 0){
		throw std::runtime_error("Port : getaddrinfo");
	}
	
//	simplified struct that is shorter version that addrinfo
//	_simp_res.sin_family = AF_INET;
//	int num = atoi(port_number);
//	_simp_res.sin_port = htons(num);
//	_simp_res.sin_addr.s_addr = INADDR_ANY;
}


Sockadrs::~Sockadrs(){
	freeaddrinfo(_res);
}

//sockaddr_in Sockadrs::getSimplRes(){
//	return _simp_res;
//}

addrinfo *	Sockadrs::getRes(){
	return _res;
}


//Sockadrs::Sockadrs(void *inp){
//
//	int status;
//	char* port_number;//this needs to be received
//	struct addrinfo hints;
//
//	port_number = (char *)inp;
//	std::memset(&hints, 0, sizeof(hints));
//	hints.ai_family = AF_INET;
//	hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
//	hints.ai_flags = AI_PASSIVE; //fill ip address by default
//	status = getaddrinfo("localhost", port_number, &hints, &_res);
//	if (status < 0){
//		throw std::runtime_error("Port : getaddrinfo");
//	}
//
////	simplified struct that is shorter version that addrinfo
////	_simp_res.sin_family = AF_INET;
////	int num = atoi(port_number);
////	_simp_res.sin_port = htons(num);
////	_simp_res.sin_addr.s_addr = INADDR_ANY;
//}
