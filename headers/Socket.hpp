//
//  Socket.hpp
//  server_xcode_project
//
//  Created by Julia Demura on 15/01/2023.
//

#ifndef Socket_hpp
#define Socket_hpp

#include "Port.hpp"

//https://pubs.opengroup.org/onlinepubs/009604499/functions/socket.html

//  Create a socket using the socket function

class Socket : public Port{
	
protected:
	int _socketFd; //opened in constructor
	
public:
	Socket(); //this will create a socket
	~Socket();
	
	void	setToNonBlocking();
	void	bindToPort();
	int		getSocketFd();
	
};

#endif /* Socket_hpp */
