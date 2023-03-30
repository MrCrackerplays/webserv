//
//  Socket.cpp
//  server_xcode_project
//
//  Created by Julia Demura on 15/01/2023.
//

#include "Socket.hpp"
#include "methods.hpp"

Socket::Socket(char * hostName, char * portNumber){
	
	int status;
	struct addrinfo hints;
	
	std::memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	status = getaddrinfo(hostName, portNumber, &hints, &_addrinfo);
	if (status < 0){
		throw std::runtime_error("Port : getaddrinfo");
	}
	_portNumber = portNumber;
	_hostName = hostName;
	_listenFd = socket(PF_INET, SOCK_STREAM, 0);
	if (_listenFd < 0){
		throw std::runtime_error("Socket : socket");
	}
}

Socket::~Socket(){
	//UNFINISHED
	//freeaddrinfo(_addrinfo); pointer being freed was not allocated - check needed
}

//work with listening socket
void	Socket::acceptNewConnect(int i){
	int newFd = 0;
	pollfd newPollfd;
	//std::cout << "start acceptNewConnect" << std::endl;
	do {
		newFd = accept(_listenFd, NULL, NULL);
		if (newFd < 0){ //not needed maybe, errno can be and issue
			if (errno != EWOULDBLOCK){
				perror("");
				close(_vFds[i].fd);
				throw std::runtime_error("SocketLoop : accept");
			}
			break;
		}
		std::cout << "new fd after accept   " << newFd << std::endl;
		newPollfd.fd = newFd;
		newPollfd.events = POLLIN;
		_vFds.push_back(newPollfd);
	} while (newFd != -1);
	//std::cout << "end acceptNewConnect" << std::endl;
}

int	findContentLenght(std::string buffer){
	
	size_t pos = buffer.find("Content-Lenght: ");
	if (pos != std::string::npos){
		pos += std::strlen("Content-Lenght: ");
		size_t endpos = buffer.find("\r\n", pos);
		if (endpos != std::string::npos){
			return std::stoi(buffer.substr(pos, endpos - pos));
		}
	}
	return false;
}

bool	fullRequestReceived(std::string buffer, size_t recvBites){
	
	size_t headrSize = buffer.find("\r\n\r\n");
	if (headrSize != std::string::npos){
		headrSize += 4;
		if (findContentLenght(buffer) == recvBites - headrSize){
			return true;
		}
	}
	return false;
}

void	Socket::recvConnection(int i){
	
	int res = 0;
	size_t headerSize;
	char buff[1024];
	std::string buffer;
	
	res = (int)recv(_vFds[i].fd, buff, 1024, 0);
	if (res < 0){
		close(_vFds[i].fd);
		throw std::runtime_error("SockedLoop : revc");
	}
	if (res == 0){
		std::cout << "connection was closed by client   " << "for fd " << _vFds[i].fd << std::endl;
		close(_vFds[i].fd);
	}
	_recvBites += res;
	_buff += buff;
	if (fullRequestReceived(_buff, _recvBites)){
		std::cout << "buffer after recv" << std::endl << std::endl << ":" << std::endl << buff << std::endl;
		
		//test now :
		std::string reply = methods(_buff, _servers, _portNumber, _hostName);
		
		
		//UNFINISHED
//		int bitesend = (int)send(_vFds[i].fd, reply.c_str(), reply.length(), 0);
//		if (bitesend < 0){
//			throw std::runtime_error("Socket : send");
//		}
		sendData(_vFds[i].fd); //test function
		close(_vFds[i].fd);
		
	}
}

void Socket::pollLoop(std::map<std::string, std::vector<Server> > servers){
	
	_servers = servers;
	setToNonBlocking(_listenFd);
	bindToPort(_listenFd, _addrinfo);
	setToListen(_listenFd);
	initiateVectPoll(_listenFd, _vFds);
	while (true) {

		if (poll(&_vFds[0], (unsigned int)_vFds.size(), 0) < 0){
			throw std::runtime_error("Socket : poll");
		} else {
			
			for (int i = 0; i < (int)_vFds.size(); i++){
				
				//std::cout << (int)_vFds.size() << std::endl;
				if (_vFds[i].fd == 0)
					continue;
				if ((_vFds[i].revents & POLLIN) == POLLIN){
					//std::cout << "unexpected result, revent should be POLLIN" << std::endl;
					if (_vFds[i].fd == _listenFd){///Listening socket is readable -> need to accept all incoming connections
						acceptNewConnect(i); //work with listening socket
					}
					else { ///connection is not on listening socket, need to be readable -> receive all data
						recvConnection(i);
					}
				}
			}

		}
	}//end of while loop

}

void	Socket::sendData(int client_socket){
	
	std::string response_header = "HTTP/1.0 200 OK\r\n"
						   "Content-Type: text/html\r\n"
						   "Content-Length: 348\r\n" // !! need to be extra careful with numbers content-lenght
	"\r\n";
	
	std::string filename = "/Users/yuliia/Codam/webserv/info_practice.html"; //HARDCODED file path
	std::ifstream file(filename);
	std::string response_body((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	std::cout << response_body << std::endl;

	//header
	int bitesend1 = (int)send(client_socket, response_header.c_str(), response_header.length(), 0);
	if (bitesend1 < 0){
		throw std::runtime_error("Socket : send");
	}
	std::cout << "header : bite sent: " <<bitesend1 << " response lenght: " << response_header.length() <<std::endl;
	
	//body from file
	int bitesend2 = (int)send(client_socket, response_body.c_str(), response_body.length(), 0);
	if (bitesend2 < 0){
		throw std::runtime_error("Socket : send");
	}
	std::cout << "body : bite sent: " <<bitesend2 << " response lenght: " << response_body.length() <<std::endl;
}

int		Socket::getSocketFd(){
	return _listenFd;
}
