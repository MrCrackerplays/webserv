#include <iostream>
#include "Server.hpp"
#include "config.hpp"
#include "Socket.hpp"
#include <fstream>
#include <poll.h>

void	setupSocket(std::map<std::string, std::vector<Server> > &servers, Socket &socket){
	setToNonBlocking(socket.getSocketFd());
	bindToPort(socket.getSocketFd(), socket.getAddrInfo());
	setToListen(socket.getSocketFd());
	initiateVectPoll(socket.getSocketFd(), socket.getPollFdVector());
	socket.setServers(servers);
}

void	pollLoop(std::vector<Socket> &vectSockets, std::map<std::string, std::vector<Server> > &servers){
	
	std::vector<Socket>::iterator it;
	for (it = vectSockets.begin(); it != vectSockets.end(); it++) {
		try {
			setupSocket(servers, *it);
		} catch (std::exception &e) {
			std::cerr << e.what() << std::endl;
		}
	}

	while (true) {
		for (it = vectSockets.begin(); it != vectSockets.end(); it++) {
			if (poll(&it->getPollFdVector()[0], (unsigned int)it->getPollFdVector().size(), 0) < 0){
				throw std::runtime_error("Socket : poll");
			} else {
				it->checkEvents();
			
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
	//config_file = "/Users/yuliia/Codam/webserv/configs/postuploadtest.conf";
	//config_file = "/Users/yuliia/Codam/webserv/configs/iframes.conf";
	
	std::map<std::string, std::vector<Server> > servers;
	std::vector<Socket> vectSockets;
	try {
		initialize_servers(servers, config_file);
		std::map<std::string, std::vector<Server> >::iterator it;
		for (it = servers.begin(); it != servers.end(); it++) {
			vectSockets.push_back(Socket((char*)it->second[0].getHost().c_str(), (char*)it->second[0].getPort().c_str()));
		}
		pollLoop(vectSockets, servers);
	} catch(const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return (1);
	}
}
