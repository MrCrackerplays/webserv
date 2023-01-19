//
//  Socket.hpp
//  server_xcode_project
//
//  Created by Julia Demura on 15/01/2023.
//

#ifndef Socket_hpp
#define Socket_hpp

#define EVENTS_NUM 5

#include "Port.hpp"
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <stdexcept>
#include <poll.h>
#include <sys/select.h>

//https://pubs.opengroup.org/onlinepubs/009604499/functions/socket.html
//  Create a socket using the socket function

class Socket : public Port{
	
protected:
	int _socketFd; //opened in constructor
	struct pollfd *_fds;
	
	void	setToNonBlocking();
	void	bindToPort();
	void	setToListen();
	void	pollLoop();
	void	sendData(int client_socket);
	void	receiveData();
	void	initiate_struct();
	void	handleEvents();
	void	incomingConnection();
	
public:
	Socket(void *inp); //this will create a socket
	~Socket();
	
	
	int		getSocketFd();
	void	setupSocket();
	
};

#endif /* Socket_hpp */
