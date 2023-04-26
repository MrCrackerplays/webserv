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
	//socket.setCGIbool(false); need to be done in client struct
	// socket.setCGIVectorSize(0);
	socket.setPollFdVectorSize(0);
}

void	pollLoop(std::vector<Socket> &vectSockets, std::map<std::string, std::vector<Server> > &servers){
	//std::cout << "pollLoop1" << std::endl;
	std::vector<Socket>::iterator it;
	for (it = vectSockets.begin(); it != vectSockets.end(); it++) {
		try {
			setupSocket(servers, *it);
		} catch (std::exception &e) {
			std::cerr << e.what() << std::endl;
		}
	}
	//std::cout << "pollLoop2" << std::endl;
	while (true) {
		std::vector<pollfd> socketsAll;
		for (it = vectSockets.begin(); it != vectSockets.end(); it++) {
			std::cout << "poll loop3" << std::endl;
			std::vector<pollfd> vFds = it->getPollFdVector();
			it->setPollFdVectorSize(vFds.size());
			
			for (int i = 0; i < (int)vFds.size(); i++) {
				socketsAll.push_back(vFds[i]);

				if (it->getCGIbool(i) == true){
					std::vector<pollfd> vCGI = it->getCGIVector(i);
					if (vCGI.size() == 1){
						socketsAll.push_back(vCGI[0]);
					} else if (vCGI.size() == 2){
						socketsAll.push_back(vCGI[0]);
						socketsAll.push_back(vCGI[1]);
					}
				}
			}
		}
		//std::cout << "poll loop4" << std::endl;
		if (poll(&socketsAll[0], (unsigned int)socketsAll.size(), 0) < 0){
			throw std::runtime_error("Socket : poll");
		
		
		} else {
			//I need to pack FD and SD back in different sockets
			
			//std::cout << "poll loop5" << std::endl;
			size_t iforAll = 0;
			//size_t itCounter = 0;
			for (it = vectSockets.begin(); it != vectSockets.end(); it++) {
    			//get the size of the original pollfd vectors

    			// size_t pollFdSize = it->getPollFdVectorSize();
				// for (size_t oneSocketCounter = 0; oneSocketCounter < pollFdSize; oneSocketCounter++)
				// {
				// 	bool isCGIinConnection = it->getCGIbool(iforAll);
				// 	if (isCGIinConnection == true){
				// 		size_t cgiSize = it->getCGIVectorSize(oneSocketCounter);
				// 		it->setPollFdVector(socketsAll.begin() + iforAll, socketsAll.begin() + iforAll + 1);
				// 		iforAll++;
				// 		if (cgiSize == 0){
				// 			break;
				// 		} else if (cgiSize == 1){
				// 			it->setCGIVector(oneSocketCounter, {socketsAll[iforAll]});
				// 			iforAll++;
				// 		} else if (cgiSize == 2){
				// 			it->setCGIVector(oneSocketCounter, {socketsAll[iforAll], socketsAll[iforAll+1]});
				// 			iforAll += 2;
				// 		}
				// 	}
					
				// }
				
    //get the size of the original pollfd vectors
    			size_t pollFdSize = it->getPollFdVectorSize();
   				for (size_t oneSocketCounter = 0; oneSocketCounter < pollFdSize; oneSocketCounter++)
    			{
        			bool isCGIinConnection = it->getCGIbool(iforAll);
        			if (isCGIinConnection == true){
            			size_t cgiSize = it->getCGIVectorSize(oneSocketCounter);
            			it->setPollFdVector(socketsAll.begin() + iforAll, socketsAll.begin() + iforAll + 1);
            			iforAll++;
            			if (cgiSize == 0){
                			break;
            			} else if (cgiSize == 1){
                			std::vector<pollfd> oneSocketCounterVec(1);
                			oneSocketCounterVec[0] = socketsAll[iforAll];
                			it->setCGIVector(oneSocketCounter, oneSocketCounterVec);
                			iforAll++;
            			} else if (cgiSize == 2){
                			std::vector<pollfd> oneSocketCounterVec(2);
               				oneSocketCounterVec[0] = socketsAll[iforAll];
                			oneSocketCounterVec[1] = socketsAll[iforAll + 1];
                			it->setCGIVector(oneSocketCounter, oneSocketCounterVec);
                			iforAll += 2;
            			}
        			}

				}


		


    			//copy the corresponding number of pollfd structures back into their respective vectors
   				//it->setPollFdVector(std::vector<pollfd>(socketsAll.begin() + i, socketsAll.begin() + i + pollFdSize));
    			//it->setCGIVector(std::vector<pollfd>(socketsAll.begin() + i + pollFdSize, socketsAll.begin() + i + pollFdSize + cgiSize));

    			//increment the iterator by the total number of pollfd structures
    			//i += pollFdSize + cgiSize;

				//check events for this socket
				//test: only one test case to test child:
				//it->setClientCGI(1);
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
