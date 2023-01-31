//
//  SocketLoop.cpp
//  server_xcode_project
//
//  Created by Julia Demura on 17/01/2023.
//

#include "Socket.hpp"
//https://www.ibm.com/docs/en/i/7.3?topic=designs-using-poll-instead-select

void	Socket::addInEventQueu(int client_socket){
	
	int allTaken = true;
	for (int i = 0; i < EVENTS_NUM; i++) {
		if (_vFds[i].fd == 0){
			_vFds[i].fd = client_socket;
			_vFds[i].events = POLLIN;
			allTaken = false;
			break;
		}
	}
	if (allTaken == true){
		//not sure if I need to clean prev sockets or how to proceed
	}
}

void	Socket::incomingConnection(){
	printf("HERE\n");
	int client_socket = accept(_listenFd, NULL, NULL);
	std::cout << "accepted: " << client_socket << std::endl;
	perror("check1");
	if (client_socket < 0){
		perror("check2");
		throw std::runtime_error(std::string("Socket : accept") + strerror(errno));
	}
	int allTaken = true;
	for (int i = 0; i < EVENTS_NUM; i++) {
		if (_vFds[i].fd == 0){
			_vFds[i].fd = client_socket;
			_vFds[i].events = POLLIN;
			allTaken = false;
			break;
		}
	}
	if (allTaken == true){
		//not sure if I need to clean prev sockets or how to proceed
	}
}

void	Socket::handleEvents(){

	for (int i = 0; i < EVENTS_NUM; i++) {
		if (_vFds[i].revents & POLLIN){

			char buff[1024];
			std::cout<< "handleEvents fd i" << i <<" now: "<< _vFds[i].fd << std::endl;
			int recvRes = (int)recv(_vFds[i].fd, buff, sizeof(buff), 0);
			std::cout << _vFds[i].fd << std::endl;
			perror("recv");
			if (recvRes < 0) {
				throw std::runtime_error("Socket : recv"); //not sure
			} else if (recvRes == 0) {
				std::cout << "Connection closed by client" << std::endl;
			} else {
				//RESPONSE
				sendData(_vFds[i].fd);
			}
		}
	}
}

void Socket::pollLoop(){

	initiateVect();
	while (true) {

		if (poll(&_vFds[0], (unsigned int)_vFds.size(), 0) < 0){
			throw std::runtime_error("Socket : poll");
		} else {

			if (_vFds[0].revents & POLLIN){
				incomingConnection();
			}
			handleEvents();

		}
	}//end of while loop

}

void	Socket::setupSocket(){

	setToNonBlocking();
	bindToPort();
	setToListen();
	pollLoop();
}


//****************** later:




void	Socket::sendData(int client_socket){

	char server_message[256] = "Hello, Client!";
	if (send(client_socket, server_message, sizeof(server_message), 0) < 0){
		throw std::runtime_error("Socket : send");
	}
}
