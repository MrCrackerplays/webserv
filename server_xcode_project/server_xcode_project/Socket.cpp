//
//  Socket.cpp
//  server_xcode_project
//
//  Created by Julia Demura on 15/01/2023.
//

#include "Socket.hpp"


Socket::~Socket(){
	//UNFINISHED
	//want to reuse here
}

void	Socket::initiateVect(){

	pollfd deflt;
	deflt.fd = 0;
	deflt.events = 0;
//	for (int i = 0; i < EVENTS_NUM; i++){
//		_vFds.push_back(deflt);
//	}
	_vFds.push_back(deflt);
	_vFds[0].fd = _listenFd;
	_vFds[0].events = POLLIN;
}

Socket::Socket(void *inp) : Sockadrs(inp){
	
	_listenFd = socket(PF_INET, SOCK_STREAM, 0);
	if (_listenFd < 0){
		throw std::runtime_error("Socket : socket");
	}
}

void	Socket::setToNonBlocking(){

/* Make socket non-blockable  */
	int flagsForFd = fcntl(_listenFd, F_GETFL, 0);
	if (flagsForFd < 0){
		throw std::runtime_error("Socket : fcntl");
	}
	if (fcntl(_listenFd, F_SETFL, flagsForFd | O_NONBLOCK) < 0){
		throw std::runtime_error("Socket : fcntl");
	}

/* Allow socket descriptor to be reuseable  */
//	int on = 1;
//	if (setsockopt(_listenFd, SOL_SOCKET,  SO_REUSEADDR, (char *)&on, sizeof(on)) < 0){
//		perror("setsockopt() failed");
//		throw std::runtime_error("setsockopt() : failed");
//	}
}

void	Socket::bindToPort(){

	if (bind(_listenFd, _res->ai_addr, _res->ai_addrlen) < 0){
		close(_listenFd);
		throw std::runtime_error("Socket : bind");
	}
}

void	Socket::setToListen(){
	
	if (listen(_listenFd, EVENTS_NUM) < 0){
		close(_listenFd);
		throw std::runtime_error("Socket : bind");
	}
}

int		Socket::getSocketFd(){
	return _listenFd;
}

//void	Socket::setListenSocket(){
//
//	setToNonBlocking();
//	bindToPort();
//	setToListen();
//}

//pollfd *		Socket::getVFd(){
//	return &_vFds[0];
//}
