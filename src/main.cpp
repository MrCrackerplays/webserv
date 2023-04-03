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
	
	
	
//	while (true) {
//
//		if (poll(&_vFds[0], (unsigned int)_vFds.size(), 0) < 0){
//			throw std::runtime_error("Socket : poll");
//		} else {
//
//			for (int i = 0; i < (int)_vFds.size(); i++){
//			//	std::cout << "VFD SIZE: " << _vFds.size() << std::endl;
//				//std::cout << (int)_vFds.size() << std::endl;
//				if (_vFds[i].fd == 0)
//					continue;
//				if ((_vFds[i].revents & POLLIN) == POLLIN){
//					//std::cout << "unexpected result, revent should be POLLIN" << std::endl;
//					if (_vFds[i].fd == _listenFd){///Listening socket is readable -> need to accept all incoming connections
//						try {
//							acceptNewConnect(i);
//						} catch (std::exception &e) {
//							//std::cerr << "failed with i = " << i << " and FD: " << _vFds[i].fd << "err message: " << e.what() << std::endl;
//						}
//											}
//					else { ///connection is not on listening socket, need to be readable -> receive all data
//						try {
//							recvConnection(i);
//						} catch (std::exception &e) {
//							//std::cerr << "failed with i = " << i << " and FD: " << _vFds[i].fd << "err message: " << e.what() << std::endl;
//						}
//
//
//					}
//				}
//			}
//
//		}
//	}//end of while loop
	
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
