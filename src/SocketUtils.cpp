//
//  SocketUtils.cpp
//  server_xcode_project
//
//  Created by Julia Demura on 17/01/2023.
//

#include "Socket.hpp"

void	initiateVectPoll(int listenFd, std::vector<pollfd> &vFds){

	pollfd deflt;
	deflt.fd = 0;
	deflt.events = 0;
//	for (int i = 0; i < EVENTS_NUM; i++){
//		_vFds.push_back(deflt);
//	}
	vFds.push_back(deflt);
	vFds[0].fd = listenFd;
	vFds[0].events = POLLIN;
}

void	setToNonBlocking(int listenFd){

/* Make socket non-blockable  */
	int flagsForFd = fcntl(listenFd, F_GETFL, 0);
	if (flagsForFd < 0){
		throw std::runtime_error("Socket : fcntl");
	}
	if (fcntl(listenFd, F_SETFL, flagsForFd | O_NONBLOCK) < 0){
		throw std::runtime_error("Socket : fcntl");
	}

/* Allow socket descriptor to be reuseable  */
	int on = 1;
	if (setsockopt(listenFd, SOL_SOCKET,  SO_REUSEADDR, (char *)&on, sizeof(on)) < 0){
		perror("setsockopt() failed");
		throw std::runtime_error("setsockopt() : failed");
	}
}

void	bindToPort(int listenFd, addrinfo *addrinfo){

	if (bind(listenFd, addrinfo->ai_addr, addrinfo->ai_addrlen) < 0){
		close(listenFd);
		throw std::runtime_error("Socket : bind");
	}
}

void	setToListen(int listenFd){
	
	if (listen(listenFd, EVENTS_NUM) < 0){
		close(listenFd);
		throw std::runtime_error("Socket : bind");
	}
}
