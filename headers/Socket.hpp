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
#include "spawnProcess.hpp"
#include "parseRequest.hpp"
#include "constructResponse.hpp"
#include "Server.hpp"
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <stdexcept>
#include <poll.h>
#include <sys/select.h>
#include <vector>
#include <fstream>
#include <stdexcept>
#include <stdio.h>
#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

struct ClientInfo {

	std::string receivedContent;
	size_t recvBytes;
	std::string reply;
	size_t biteToSend;

	parsRequest ClientRequest;
	response ClientResponse;

	bool isCGI;
	bool CgiDone;
	CGIInfo cgiInfo;
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


	// size_t _vCGISizeTemp;
	// std::vector<pollfd> &_vCGITemp;
	
	
public:
	Socket(char * hostName, char * portNumber);
	~Socket();
	
	//EVENTS_METHODS_________________________________________________
	void	checkEvents();
	void	acceptNewConnect(int i);
	void	recvConnection(int i);
	void	sendData(int client_socket);
	void	checkCGIevens(int i);
	void	closeClientConnection(int i);


	//CGI_METHODS____________________________________________________
	void	CGIerrorReply(int i);
	void	writeInChild(int i);
	void	readFromChild(int i);
	void	startChild(int i);
	void	pickCGIState(int i);
	
	
	//GETTERS________________________________________________________
	
	int		getSocketFd();
	addrinfo *getAddrInfo();

	std::vector<pollfd> &getPollFdVector();
	size_t	getPollFdVectorSize();
	

	std::vector<pollfd> &getCGIVector(int i);
	size_t	getCGIVectorSize(int i);
	bool	getCGIbool(int i);
	size_t numberOfConnections(int i);
	
	//SETTRES________________________________________________________
	void	setServers(std::map<std::string, std::vector<Server> > &servers);

	
//old parts
	void setPollFdVector(std::vector<pollfd> vFds);


    // void setCGIVector(std::vector<pollfd> cgiFds) {
    //     _vCGITemp = std::move(cgiFds);
    // }

	void setPollFdVectorSize(size_t size);
	void setPollFdVector1(int clientInd, const std::vector<pollfd>& pollFdVec);
	void setCGIVector(int clientInd, const std::vector<pollfd>& cgiFdVec);
	void unpackVectorintoSocket(size_t &allCounter, size_t fdCounter, std::vector<pollfd> socketsAll);

};

void	initiateVectPoll(int listenFd, std::vector<pollfd> &vFds);
void	setToNonBlocking(int listenFd);
void	bindToPort(int listenFd, addrinfo *addrinfo);
void	setToListen(int listenFd);


#endif /* Socket_hpp */


    // void setCGIVector(int clientInd, const std::vector<pollfd>& cgiFdVec) {

	// 	CGIInfo &cgiInfo = _clients[clientInd].cgiInfo;
	// 	if (cgiInfo.vCGI.size() == 1) {
	// 		cgiInfo.vCGI[0] = std::move(cgiFd0);

	// 	} else if (cgiInfo.vCGI.size() == 2) {
	// 		cgiInfo.vCGI[0] = std::move(cgiFd0);
	// 		cgiInfo.vCGI[1] = std::move(cgiFd1);
	// 	}
    // }

	// void setCGIVector(int clientInd, const std::vector<pollfd>& cgiFdVec) {
    	
	// 	CGIInfo& cgiInfo = _clients[clientInd].cgiInfo;
   	// 	if (cgiInfo.vCGI.size() == 1) {
    //     	cgiInfo.vCGI[0] = std::move(cgiFdVec[0]);

    // 	} else if (cgiInfo.vCGI.size() == 2) {
    //    		cgiInfo.vCGI[0] = std::move(cgiFdVec[0]);
    //   		cgiInfo.vCGI[1] = std::move(cgiFdVec[1]);
    // 	}
	// }

	// void setCGIVectorSize(size_t size) {
	// 	_vCGISizeTemp = size;
	// }