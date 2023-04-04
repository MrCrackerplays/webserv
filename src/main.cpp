#include <iostream>
#include "Server.hpp"
#include "config.hpp"
#include "Socket.hpp"
#include <fstream>
#include <poll.h>


void	setupSocket(Socket &socket){
	
	setToNonBlocking(socket.getSocketFd());
	bindToPort(socket.getSocketFd(), socket.getAddrInfo());
	setToListen(socket.getSocketFd());
	initiateVectPoll(socket.getSocketFd(), socket.getPollFdVector());
}

void	pollLoop(std::vector<Socket> &vectSockets, std::map<std::string, std::vector<Server> > &servers){
	
	for (std::vector<Socket>::iterator it = vectSockets.begin(); it != vectSockets.end(); it++) {
		try {
			setupSocket(*it);
		} catch (std::exception &e) {
				std::cerr << e.what() << std::endl;
		}
	}

	while (true) {
		for (std::vector<Socket>::iterator it = vectSockets.begin(); it != vectSockets.end(); it++) {
			Socket currentSocket = *it;
			if (poll(&currentSocket.getPollFdVector()[0], (unsigned int)currentSocket.getPollFdVector().size(), 0) < 0){
				throw std::runtime_error("Socket : poll");
			} else {
				currentSocket.checkEvents();
				
			}
		}
	}
	
}


int	main(int argc, char **argv) {
	
	std::string	config_file = "configs/default.conf";
	if (argc > 2) {
		std::cout << "invalid number of arguments" << std::endl;
		return (1);
	}
	if (argc == 2)
		config_file = argv[1];
	
	//test yuliia // comment if not needed :
	config_file = "/Users/yuliia/Codam/webserv/configs/postuploadtest.conf";
	
	std::map<std::string, std::vector<Server> > servers;
	std::map<std::string, Socket> sockets;
	std::vector<Socket> vectSockets;
	try {
		initialize_servers(servers, config_file);
		std::map<std::string, std::vector<Server> >::iterator it;
		for (it = servers.begin(); it != servers.end(); it++)
		{
			//std::cout << it->second[0].getHost() << ":" << it->second[0].getPort() << std::endl;
//			sockets.insert(std::pair<std::string, Socket>(it->first,
			vectSockets.push_back(Socket((char*)it->second[0].getHost().c_str(), (char*)it->second[0].getPort().c_str())) ;

		}
		pollLoop(vectSockets, servers);
		
		
		std::map<std::string, Socket>::iterator it2;
		for (it2 = sockets.begin(); it2 != sockets.end(); it2++) {
			std::cout << "Starting socket [" << it2->first << "]" << std::endl;
			//it2->second.pollLoop(servers);
		}
	} catch(const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return (1);
	}
}
