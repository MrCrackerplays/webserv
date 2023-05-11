#include <iostream>
#include "Server.hpp"
#include "config.hpp"
#include "Socket.hpp"
#include <fstream>
#include <poll.h>
#include <csignal>


void	setupSocket(std::map<std::string, std::vector<Server> > &servers, Socket &socket){
	setToNonBlocking(socket.getSocketFd());
	bindToPort(socket.getSocketFd(), socket.getAddrInfo());
	setToListen(socket.getSocketFd());
	initiateVectPoll(socket.getSocketFd(), socket.getPollFdVector());
	socket.setServers(servers);
	socket.setPollFdVectorSize(0);
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
	std::vector<struct pollfd> socketsAll;
	
	while (true) {
		socketsAll.clear();
		for (it = vectSockets.begin(); it != vectSockets.end(); it++) {
			
			std::vector<struct pollfd> &vFds = it->getPollFdVector();
			it->setPollFdVectorSize(vFds.size());
			for (size_t i = 0; i < vFds.size(); i++) {
				socketsAll.push_back(vFds[i]);
				if (it->getCGIbool(i) == true){
					std::vector<struct pollfd> &vCGI = it->getCGIVector(i);
					for (size_t j = 0; j < vCGI.size(); j++) {
						socketsAll.push_back(vCGI[j]);
					}
				}
			}
		}

		if (poll(&socketsAll[0], (unsigned int)socketsAll.size(), 1000) < 0){
			throw std::runtime_error("Socket : poll");
		
		} else { 
			size_t iAll = 0;
			
			for (it = vectSockets.begin(); it != vectSockets.end(); it++) {
				size_t nClients = it->getPollFdVectorSize();
				for (size_t n = 0; n < nClients; n++) {
					it->unpackVectorintoSocket(iAll, n, socketsAll);
				}
				it->checkEvents();
			}
		}
	}

}


int	main(int argc, char **argv) {
	
	char str[PATH_MAX];
	getcwd(str, PASS_MAX);
	std::cout << str << std::endl;
	
	std::string	config_file = "configs/default.conf";
	if (argc > 2) {
		std::cout << "invalid number of arguments" << std::endl;
		return (1);
	}
	if (argc == 2)
		config_file = argv[1];

	//ignore SIGPIPE which sometimes happens when client closes connection
	std::signal(SIGPIPE, SIG_IGN);
	
	std::map<std::string, std::vector<Server> > servers;
	std::vector<Socket> vectSockets;
	try {
		initialize_servers(servers, config_file);
		std::map<std::string, std::vector<Server> >::iterator it;
		for (it = servers.begin(); it != servers.end(); it++) {
			vectSockets.push_back(Socket((char*)it->second[0].getHost().c_str(), (char*)it->second[0].getPort().c_str()));
		}
		std::vector<Socket>::iterator stupid_vector;
		for (stupid_vector = vectSockets.begin(); stupid_vector != vectSockets.end(); stupid_vector++) {
			stupid_vector->createAddrinfo();
		}
		pollLoop(vectSockets, servers);
	} catch(const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return (1);
	}
}
