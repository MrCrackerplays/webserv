//
//  Socket.hpp
//  server_xcode_project
//
//  Created by Julia Demura on 15/01/2023.
//

#ifndef Socket_hpp
#define Socket_hpp

#define EVENTS_NUM 5

//#include "Sockadrs.hpp"
#include "Server.hpp"
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <stdexcept>
#include <poll.h>
#include <sys/select.h>
#include <vector>
#include <fstream>
#include "parseRequest.hpp"
#include <stdexcept>
#include <stdio.h>
#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

//https://pubs.opengroup.org/onlinepubs/009604499/functions/socket.html

class Socket{
	
protected:
	
	//adding servers
	std::map<std::string, std::vector<Server> > _servers;
	
	//transfering addinfo into socket
	struct addrinfo *_addrinfo;
	std::string _hostName;
	std::string _portNumber;
	
	//old
	int _listenFd;
	std::vector<pollfd> _vFds;
	//std::vector<std::string> _buffer; - suggested that std::string will be enough, but testing needed
	std::string _buff;
	size_t _recvBites;

	void	sendData(int client_socket); //will be heavily adjusted 
	void	acceptNewConnect(int i);
	void	recvConnection(int i);
	
	
public:
	Socket(char * hostName, char * portNumber);
	~Socket();
	int		getSocketFd();
	void	pollLoop(std::map<std::string, std::vector<Server> > servers);
};

void	initiateVectPoll(int listenFd, std::vector<pollfd> &vFds);
void	setToNonBlocking(int listenFd);
void	bindToPort(int listenFd, addrinfo *addrinfo);
void	setToListen(int listenFd);

#endif /* Socket_hpp */
