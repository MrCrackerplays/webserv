//
//  Socket.hpp
//  server_xcode_project
//
//  Created by Julia Demura on 15/01/2023.
//

#ifndef Socket_hpp
#define Socket_hpp

#define EVENTS_NUM 5
#define MAX_REQUEST_SIZE 8192

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

struct ClientInfo {

	//int sd;
	std::string receivedContent;
	size_t recvBytes;
	std::string reply;
	size_t biteToSend;
};

class Socket{
	
protected:
	
	//adding pair reply_string-client_socket
	std::vector<ClientInfo> _clients;
	
	//adding servers
	std::map<std::string, std::vector<Server> > *_servers;
	
	//transfering addinfo into socket
	struct addrinfo *_addrinfo;
	std::string _hostName;
	std::string _portNumber;
	
	int _listenFd;
	std::vector<pollfd> _vFds;
	size_t _vFdsSize;
	std::vector<pollfd> _vCGI;
	size_t _vCGISize;
	
	
public:
	void	closeClientConnection(int i);
	Socket(char * hostName, char * portNumber);
	~Socket();
	
	//event methods
	void	checkEvents();
	void	acceptNewConnect(int i);
	void	recvConnection(int i);
	void	sendData(int client_socket);
	
	//getters
	int		getSocketFd();
	addrinfo *getAddrInfo();
	std::vector<pollfd> &getPollFdVector();
	std::vector<pollfd> &getCGIVector();
	size_t	getPollFdVectorSize();
	size_t	getCGIVectorSize();

	
	//setters
    void setPollFdVector(std::vector<pollfd> vFds) {
        _vFds = std::move(vFds);
    }
    void setCGIVector(std::vector<pollfd> cgiFds) {
        _vCGI = std::move(cgiFds);
    }
	void setPollFdVectorSize(size_t size) {
		_vFdsSize = size;
	}
	void setCGIVectorSize(size_t size) {
		_vCGISize = size;
	}
	void	setServers(std::map<std::string, std::vector<Server> > &servers);
};

void	initiateVectPoll(int listenFd, std::vector<pollfd> &vFds);
void	setToNonBlocking(int listenFd);
void	bindToPort(int listenFd, addrinfo *addrinfo);
void	setToListen(int listenFd);



#endif /* Socket_hpp */
