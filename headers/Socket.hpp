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
#define TIMEOUT_CGI 1
#define TIMEOUT_CLIENT 20

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
#include <ctime>
#include <signal.h>

typedef struct {

	std::string receivedContent;
	std::vector<char> receivedContentVector;
	ssize_t recvBytes;
	std::string reply;
	ssize_t biteToSend;
	long int biteSentInt;
	parsRequest ClientRequest;
	response ClientResponse;
	bool isCGI;
	bool CgiDone;
	std::string cgiReply;
	CGIInfo cgiInfo;
	std::time_t startTime;
	bool timerOn;
	bool timeout;

} ClientInfo;

class Socket{
	
protected:
	
	std::vector<ClientInfo> _clients;
	std::map<std::string, std::vector<Server> > *_servers;
	struct addrinfo *_addrinfo;
	std::string _hostName;
	std::string _portNumber;
	int _listenFd;
	std::vector<struct pollfd> _vFds;
	size_t _vFdsSize;
	
public:
	
	void	printClientFds();
	void	createAddrinfo();
	
	Socket(char * hostName, char * portNumber);
	~Socket();
	
	//EVENTS_METHODS_________________________________________________
	void	checkEvents();
	void	acceptNewConnect(int i);
	void	recvConnection(int i);
	void	sendData(int client_socket);
	void	checkCGIevens(int i);
	void	closeClientConnection(int i);
	void	checkOnChild(int i);
	void	clientTimeout(int i);


	//CGI_METHODS____________________________________________________
	void	CGIerrorReply(int i);
	void	writeInChild(int i);
	void	readFromChild(int i);
	void	startChild(int i);
	void	pickCGIState(int i);
	
	
	//GETTERS________________________________________________________
	
	int		getSocketFd();
	addrinfo *getAddrInfo();

	std::vector<struct pollfd> &getPollFdVector();
	size_t	getPollFdVectorSize();
	

	std::vector<struct pollfd> &getCGIVector(int i);
	size_t	getCGIVectorSize(int i);
	bool	getCGIbool(int i);
	size_t	numberOfConnections();
	
	//SETTRES________________________________________________________
	void	setServers(std::map<std::string, std::vector<Server> > &servers);
	void	setPollFdVectorSize(size_t size);
	void	unpackVectorintoSocket(size_t &allCounter, size_t fdCounter, std::vector<struct pollfd> &socketsAll);


};

void	initiateVectPoll(int listenFd, std::vector<struct pollfd> &vFds);
void	setToNonBlocking(int listenFd);
void	bindToPort(int listenFd, struct addrinfo *addrinfo);
void	setToListen(int listenFd);


#endif /* Socket_hpp */
