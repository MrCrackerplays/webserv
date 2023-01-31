//
//  main.cpp
//  server_xcode_project
//
//  Created by Julia Demura on 09/01/2023.
//

#include <iostream>
#include "Socket.hpp"
#include <fstream>


//void	separate_for_test(char *port_number){
//
//	//create sockaddr_in
//	struct sockaddr_in addrStruct;
//	addrStruct.sin_family = AF_INET;
//	int num = atoi(port_number);
//	addrStruct.sin_port = htons(num);
//	addrStruct.sin_addr.s_addr = INADDR_ANY;
//
//
//	//create socket
//	int socketFd = socket(PF_INET, SOCK_STREAM, 0);
//	if (socketFd < 0){
//		throw std::runtime_error("Socket : socket");
//	}
//
//	// bind
//	if (bind(socketFd, (sockaddr*)&addrStruct, sizeof(addrStruct)) < 0){
//		close(socketFd);
//		throw std::runtime_error("Socket : bind");
//	} else {
//		std::cout << "bind sucsessful"  << std::endl;
//	}
//
//	//listen
//	if (listen(socketFd, SOMAXCONN) < 0){
//		close(socketFd);
//		throw std::runtime_error("Socket : bind");
//	}
//	//set to non blocking
//	if (fcntl(socketFd, F_SETFL, O_NONBLOCK) < 0){
//		throw std::runtime_error("Socket : fcntl");
//	}
//
//	//poll
//	struct pollfd *fds;
//	fds = (pollfd *)malloc(sizeof(pollfd) * 5);
//
//	fds->fd = socketFd;
//	fds->events = POLLIN;
//
//	int res = poll(fds, 1, 0);
//	perror("CHECK poll");
//	if (res < 0){
//		throw std::runtime_error("Socket : poll");
//	} else if (res == 0){
//		//Returns 0 if no descriptors are ready before the timer expires
//		throw std::runtime_error("Socket : poll no FD");
//	} else {
//
//		if (fds[0].revents & POLLIN){
//			///one or more file descriptors have events available.
//			///The code checks if the revents field of the fds struct contains the POLLIN flag,
//			///which indicates that the file descriptor has incoming data.
//
//		}
//	}
//}


int main(int argc, const char * argv[]) {

	if (argc != 2){
		std::cout << "not enough arguments" << std::endl;
	}

	argv[1] = "7777";
	//separate_for_test((char *)argv[1]);

	//test Socket
	try {
		std::fstream fs;
		fs.open("/Users/yuliia/Codam/webserv/info_serv.strings", std::fstream::in);

		Socket test((void *)argv[1]);
		int fd = test.getSocketFd();
		printf("FD: %i\n", fd);
		test.setupSocket();

		//acceptServ(test.getSocketFd());
	} catch (std::exception &e) {
		std::cerr << e.what() << std::endl;
	}


	return 0;
}
