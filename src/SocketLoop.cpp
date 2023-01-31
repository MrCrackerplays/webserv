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

//work with listening socket
void	Socket::acceptNewConnect(int i){
	int newFd = 0;
	pollfd newPollfd;

	do {
		newFd = accept(_listenFd, NULL, NULL);
		if (newFd < 0){
			if (errno != EWOULDBLOCK){ ///EWOULDBLOCK is returned when you are using a non-blocking socket and there is not enough space in the kernel's outgoing-data-buffer for that socket to hold any of the data you want to send. ->> https://stackoverflow.com/questions/16281468/write-to-the-client-returns-ewouldblock-when-server-is-slow
				perror("");
				throw std::runtime_error("SocketLoop : accept");
			}
			break;
		}
		newPollfd.fd = newFd;
		newPollfd.events = POLLIN;
		_vFds.push_back(newPollfd);
	} while (newFd != -1);
}

void	Socket::recNewConnect(int i){
	int res = 0;
	char buff[1024];
	
	do {
		res = (int)recv(_vFds[i].fd, buff, sizeof(buff), 0);
		if (res < 0){
			if (errno != EWOULDBLOCK){
				perror("recv");
				throw std::runtime_error("SockedLoop : revc");
			}
			break;
		}
		else if (res == 0){ //connection was closed by client
			break;
		}
		else{
			//here can be different actions
			sendData(_vFds[i].fd);
			
			break;
		}
		
		
	} while (true);
	
}

void Socket::pollLoop(){

	initiateVect();
	while (true) {

		if (poll(&_vFds[0], (unsigned int)_vFds.size(), 0) < 0){
			throw std::runtime_error("Socket : poll");
		} else {
			
			for (int i = 0; i < (int)_vFds.size(); i++){
				if (_vFds[i].fd == 0)
					continue;
				if (_vFds[i].revents != POLLIN){
					//std::cout << "unexpected result, revent should be POLLIN" << std::endl;
					break;
				}
				if (_vFds[i].fd == _listenFd){///Listening socket is readable -> need to accept all incoming connections
					acceptNewConnect(i); //work with listening socket
				}
				
				else { ///connection is not on listening socket, need to be readable -> receive all data
					recNewConnect(i);
				}
			}

			

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
