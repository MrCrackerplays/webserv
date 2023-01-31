//
//  Socket.hpp
//  server_xcode_project
//
//  Created by Julia Demura on 15/01/2023.
//

#ifndef Socket_hpp
#define Socket_hpp

#define EVENTS_NUM 5

#include "Sockadrs.hpp"
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <stdexcept>
#include <poll.h>
#include <sys/select.h>
#include <vector>

//https://pubs.opengroup.org/onlinepubs/009604499/functions/socket.html
//  Create a socket using the socket function

class Socket : public Sockadrs{
	
protected:
	int _listenFd; //opened in constructor
	//struct pollfd *_fds;
	std::vector<pollfd> _vFds;
	
	
	void	setToNonBlocking();
	void	bindToPort();
	void	setToListen();
	void	addInEventQueu(int client_socket);
	void	initiateVect();
	
	
	void	pollLoop();
	void	sendData(int client_socket);
	void	receiveData();
	void	initiateStruct();
	void	handleEvents();
	void	incomingConnection();
	
	
	
public:
	Socket(void *inp); //this will create a socket
	~Socket();
	int		getSocketFd();
	void	setupSocket();
	
	//void	setListenSocket();
	//pollfd *		getVFd();
};

#endif /* Socket_hpp */
