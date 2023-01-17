//
//  Socket.cpp
//  server_xcode_project
//
//  Created by Julia Demura on 15/01/2023.
//

#include "Socket.hpp"

Socket::Socket(void *inp) : Port(inp){
	
	_socketFd = socket(_res->ai_family, _res->ai_socktype, _res->ai_protocol);
	if (_socketFd < 0){
		throw std::runtime_error("Socket : socket");
//		perror("failed to create socket: socket");
//		exit(1);
	}
}

Socket::~Socket(){
	
	//not sure if here I should close socket fd
	close(_socketFd);
}


int		Socket::getSocketFd(){
	return _socketFd;
}


void	Socket::acceptServ(){

		
	//	if (accept(_socketFd, _res->ai_addr, _res->ai_addrlen) < 0){
	//		throw std::runtime_error("Socket : accept");
	//	}

		int client_socket = accept(_socketFd, NULL, NULL);
		if (client_socket < 0){
			printf("err %s : %i\n", strerror(errno), errno);
			throw std::runtime_error("Socket : accept");
			//[EAGAIN] The socket is marked non-blocking, and the receive operation would block, or a receive timeout had been set, and the timeout expired before data were received.
		}
	
	///The error code "Resource temporarily unavailable" (errno value 35) can occur when calling the accept function in a non-blocking socket. This error indicates that the socket is in non-blocking mode and there are no incoming connections to be accepted at the moment.

	///When the socket is in non-blocking mode, the accept function returns immediately with the error code EAGAIN or EWOULDBLOCK (both have the same value) when there are no incoming connections to be accepted. In this case, you can use the select, poll or epoll system call to wait for incoming connections before calling accept again
	
		char server_message[256] = "Hello, Client!";
		
		if (send(client_socket, server_message, sizeof(server_message), 0) < 0){
			throw std::runtime_error("Socket : send");
			
		}

}




void	Socket::setupSocket(){
	
	setToNonBlocking();
	bindToPort();
	
	
	//do poll
	
	//setToListen();
	//acceptServ();
	
	
	//		while (1)
	//			continue;
}
