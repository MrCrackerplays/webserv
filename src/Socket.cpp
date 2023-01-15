//
//  Socket.cpp
//  server_xcode_project
//
//  Created by Julia Demura on 15/01/2023.
//

#include "Socket.hpp"
#include <unistd.h>
#include <fcntl.h>

Socket::Socket(){
	
	_socketFd = socket(_res->ai_family, _res->ai_socktype, _res->ai_protocol);
	if (_socketFd < 0){
		perror("failed to create socket");
		exit(1);
	}
}

Socket::~Socket(){
	
	//not sure if here I should close socket fd
	close(_socketFd);
}


//setting the file descriptor associated with the socket (sockfd) to non-blocking mode. The fcntl function is used to modify the properties of the file descriptor.
//F_SETFL: set the file status flags.
//flags | O_NONBLOCK: This is the new value for the file status flags. flags is the value retrieved from the previous fcntl(sockfd, F_GETFL, 0) call. O_NONBLOCK is a constant defined in the <fcntl.h> header, it sets the non-blocking mode on the file descriptor.
//The | operator is a bitwise OR operator. It sets the bit of O_NONBLOCK to 1, while keeping the current value of other flags (flags) intact.
void	Socket::setToNonBlocking(){
	
	int flagsForFd = fcntl(_socketFd, F_GETFL, 0);
	if (flagsForFd < 0){
		perror("failed to create socket");
		exit(1);
	}
	if (fcntl(_socketFd, F_SETFL, flagsForFd | O_NONBLOCK) < 0){
		perror("failed to create socket");
		exit(1);
	}
}

void	Socket::bindToPort(){
	
	
	
}



int		Socket::getSocketFd(){
	return _socketFd;
}
