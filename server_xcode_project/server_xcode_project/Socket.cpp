//
//  Socket.cpp
//  server_xcode_project
//
//  Created by Julia Demura on 15/01/2023.
//

#include "Socket.hpp"



void	Socket::sendData(int client_socket){

	char server_message[256] = "Hello, Client!";
	if (send(client_socket, server_message, sizeof(server_message), 0) < 0){
		throw std::runtime_error("Socket : send");

	}
}

void	Socket::incomingConnection(){
	
	//incloming connection, explanation of '&' operation in info
	int client_socket = accept(_socketFd, NULL, NULL);
	if (client_socket < 0){
		throw std::runtime_error("Socket : accept"); //not sure
	}
	// add the new client socket to the list of file descriptors being monitored
	for (int i = 0; i < EVENTS_NUM; i++) {
		if (_fds[i].fd == 0){
			_fds[i].fd = client_socket;
			_fds[i].events = POLLIN;
			break;
		}
	}
}

void	Socket::handleEvents(){

	for (int i = 0; i < EVENTS_NUM; i++) {
		if (_fds[i].fd == 0){
			continue;
		}
		if (_fds[i].revents & POLLIN){
			char buff[1024];
			int recvRes = (int)recv(_fds[i].fd, buff, sizeof(buff), 0);
			
			if (recvRes < 0) {
				throw std::runtime_error("Socket : accept"); //not sure
			} else if (recvRes == 0) {
				std::cout << "Connection closed by client" << std::endl;
				close(_fds[i].fd);
			} else {
				//RESPONSE
				sendData(_fds[i].fd);
			}
		}
	}
	
}



void Socket::pollLoop(){
	
	initiate_struct();
	while (true) {
		
		if (poll(_fds, EVENTS_NUM, -1) < 0){
			free(_fds);
			close(_socketFd);
			throw std::runtime_error("Socket : poll");
		} else {
			
			if (_fds[0].revents & POLLIN){
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
