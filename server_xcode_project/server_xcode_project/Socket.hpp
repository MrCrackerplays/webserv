//
//  Socket.hpp
//  server_xcode_project
//
//  Created by Julia Demura on 15/01/2023.
//

#ifndef Socket_hpp
#define Socket_hpp

#include "Port.hpp"
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <stdexcept>

//https://pubs.opengroup.org/onlinepubs/009604499/functions/socket.html
//  Create a socket using the socket function

class Socket : public Port{
	
protected:
	int _socketFd; //opened in constructor
	void	setToNonBlocking();
	void	bindToPort();
	void	setToListen();
	
public:
	Socket(void *inp); //this will create a socket
	~Socket();
	
	void	acceptServ();
	int		getSocketFd();
	void	setupSocket();
	
};

#endif /* Socket_hpp */
