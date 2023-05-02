//
//  SocketUtils.cpp
//  server_xcode_project
//
//  Created by Julia Demura on 17/01/2023.
//

#include "Socket.hpp"

void	Socket::printClientFds(){
	
	for (size_t i = 0; i < _vFds.size(); i++){
		std::cout <<  "FD["<< i <<"] == " << _vFds[i].fd << " | event ==" << _vFds[i].events << " | revent ==" << _vFds[i].revents << std::endl;
		std::cout << "does it has CG? " << _clients[i].isCGI << std::endl;
		if (_clients[i].isCGI == true){
			std::cout << "it has CGI" << std::endl;
			std::cout << "CgiFd[0] == "<< _clients[i].cgiInfo.vCGI[0].fd << " | event ==" << _clients[i].cgiInfo.vCGI[0].events << " | revent ==" << _clients[i].cgiInfo.vCGI[0].events << std::endl;
			std::cout << "CgiFd[1] == "<< _clients[i].cgiInfo.vCGI[1].fd << " | event ==" << _clients[i].cgiInfo.vCGI[1].events << " | revent ==" << _clients[i].cgiInfo.vCGI[1].events << std::endl;
		}
	}
}

void	initiateVectPoll(int listenFd, std::vector<struct pollfd> &vFds){

	struct pollfd deflt;
	deflt.fd = 0;
	deflt.events = 0;
	vFds.push_back(deflt);
	vFds[0].fd = listenFd;
	vFds[0].events = POLLIN; //pollin only or POLLHUP as well?
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

void	bindToPort(int listenFd, struct addrinfo *addrinfo){

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
