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

//**********************************************************

//void	Socket::sendData(int client_socket){
//
//	char server_message[256] = "Hello, Client!";
//	if (send(client_socket, server_message, sizeof(server_message), 0) < 0){
//		throw std::runtime_error("Socket : send");
//	}
//}

//int	incomingConnection(int listenFd){
//	int client_socket = accept(listenFd, NULL, NULL);
//	std::cout << "accepted: " << client_socket << std::endl;
//	if (client_socket < 0){
//		perror("Socket : accept");
//		throw std::runtime_error(std::string("Socket : accept") + strerror(errno));
//	}
//	return client_socket;
//}
//
//void	handleEvents(Socket *Skt){
//
//	for (int i = 0; i < EVENTS_NUM; i++) {
//		if (Skt->getVFd()[i].revents & POLLIN){
//
//			char buff[1024];
//			std::cout<< "handleEvents fd i" << i <<" now: "<< Skt->getVFd()[i].fd << std::endl;
//			int recvRes = (int)recv(Skt->getVFd()[i].fd, buff, sizeof(buff), 0);
//			std::cout << Skt->getVFd()[i].fd << std::endl;
//			perror("recv");
//			if (recvRes < 0) {
//				throw std::runtime_error("Socket : recv"); //not sure
//			} else if (recvRes == 0) {
//				std::cout << "Connection closed by client" << std::endl;
//			} else {
//				//RESPONSE
//				//sendData(_vFds[i].fd);
//			}
//		}
//	}
//}

//void	addInEventQueu(Socket *Skt, int client_socket){
//
//	int allTaken = true;
//	for (int i = 0; i < EVENTS_NUM; i++) {
//		if (Skt->getVFd()[i].fd == 0){
//			Skt->getVFd()[i].fd = client_socket;
//			Skt->getVFd()[i].events = POLLIN;
//			allTaken = false;
//			break;
//		}
//	}
//	if (allTaken == true){
//		//not sure if I need to clean prev sockets or how to proceed
//	}
//}

//void pollLoop(const char *port){
//
//	Socket Skt((void *)port);
//	int incomingFd = 0;
//
//	Skt.initiateVect();
//	while (true) {
//
//		if (poll(Skt.getVFd(), EVENTS_NUM, 0) < 0){
//			throw std::runtime_error("Socket : poll");
//		} else {
//			//std::cout << Skt.getVFd()[0].revents <<std::endl;
//			if (Skt.getVFd()[0].revents & POLLIN){
//				incomingFd = incomingConnection(Skt.getVFd()[0].fd);
//				//addInEventQueu(&Skt, incomingFd);
//
//			}
//			handleEvents(&Skt);
//
//		}
//	}//end of while loop
//
//}

//********************************************

void	smallTest(const char *port){
	try {
		std::fstream fs;
		fs.open("/Users/yuliia/Codam/webserv/info_serv.strings", std::fstream::in);

		Socket test((void *)port);
		int fd = test.getSocketFd();
		printf("FD: %i\n", fd);
		test.setupSocket();

		//acceptServ(test.getSocketFd());
	} catch (std::exception &e) {
		std::cerr << e.what() << std::endl;
	}
}

int main(int argc, const char * argv[]) {

	if (argc != 2){
		std::cout << "not enough arguments" << std::endl;
	}
	//separate_for_test((char *)argv[1]);
	smallTest("7777");
	//pollLoop("7777");

	return 0;
}
