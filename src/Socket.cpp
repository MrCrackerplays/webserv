//
//  Socket.cpp
//  server_xcode_project
//
//  Created by Julia Demura on 15/01/2023.
//

#include "Socket.hpp"
#include "methods.hpp"

int		Socket::getSocketFd(){
	return _listenFd;
}

addrinfo *	Socket::getAddrInfo(){
	return _addrinfo;
}

std::vector<pollfd> &Socket::getPollFdVector(){
	return _vFds;
}

Socket::Socket(char * hostName, char * portNumber){
	
	int status;
	struct addrinfo hints;
	
	std::memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	
	_addrinfo = new addrinfo;
	
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
	freeaddrinfo(_addrinfo); //pointer being freed after allocating with new - check needed
}

//work with listening socket
void	Socket::acceptNewConnect(int i){
	int newFd = 0;
	pollfd newPollfd;
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
		if ((unsigned int)findContentLenght(buffer) == recvBites - headrSize){
			return true;
		}
	}
	return false;
}

void	Socket::recvConnection(int i){
	
	int res = 0;
	char buff[1024];
	std::string buffer;
	std::cout << _vFds[i].fd << std::endl;
	
	res = (int)recv(_vFds[i].fd, buff, 1024, 0);

	if (res < 0){
		close(_vFds[i].fd);

		std::cerr << "recv failed on i = " << i << "FD: " << _vFds[i].fd << strerror(errno) << std::endl;
		throw std::runtime_error("SockedLoop : recv");
	}
	if (res == 0){
		std::cout << "connection was closed by client   " << "for fd " << _vFds[i].fd << std::endl;
		close(_vFds[i].fd);
		_vFds.erase(_vFds.begin() + i);
	}
	_recvBites += res;
	_buff += buff;
	if (fullRequestReceived(_buff, _recvBites)){
		//std::cout << "buffer after recv" << std::endl << std::endl << ":" << std::endl << buff << std::endl;
		
		//test now :
		std::string reply = methods(_buff, _servers, _portNumber, _hostName);
		
		//UNFINISHED
		int bitesend = (int)send(_vFds[i].fd, reply.c_str(), reply.length(), 0);
		if (bitesend < 0){
			throw std::runtime_error("Socket : send");
		}
		//sendData(_vFds[i].fd); //test function

		std::cout << "reply was sent" << std::endl;
		close(_vFds[i].fd);
		// close(_vFds[i + 1].fd);
		_vFds.erase(_vFds.begin() + i);
		_recvBites = 0;
		_buff.clear();
	}
}

void	Socket::checkEvents(){
	
	for (int i = 0; i < (int)_vFds.size(); i++){
		
		if (_vFds[i].fd == 0)
			continue;
		if ((_vFds[i].revents & POLLIN) == POLLIN){
			if (_vFds[i].fd == _listenFd){///Listening socket is readable -> need to accept all incoming connections
				try {
					acceptNewConnect(i);
					} catch (std::exception &e) {
							std::cerr << "failed with i = " << i << " and FD: " << _vFds[i].fd << "err message: " << e.what() << std::endl;
							}
												}
		else { ///connection is not on listening socket, need to be readable -> receive all data
				try {
					recvConnection(i);
					} catch (std::exception &e) {
							std::cerr << "failed with i = " << i << " and FD: " << _vFds[i].fd << "err message: " << e.what() << std::endl;
							}
						}
					}
				}
}




//void Socket::pollLoop(std::map<std::string, std::vector<Server> > servers){
//
//	_servers = servers;
//	try {
//		setToNonBlocking(_listenFd);
//		bindToPort(_listenFd, _addrinfo);
//		setToListen(_listenFd);
//		initiateVectPoll(_listenFd, _vFds);
//	} catch (std::exception &e) {
//		std::cerr << e.what() << std::endl;
//	}
//	while (true) {
//
//		if (poll(&_vFds[0], (unsigned int)_vFds.size(), 0) < 0){
//			throw std::runtime_error("Socket : poll");
//		} else {
//
//			for (int i = 0; i < (int)_vFds.size(); i++){
//			//	std::cout << "VFD SIZE: " << _vFds.size() << std::endl;
//				//std::cout << (int)_vFds.size() << std::endl;
//				if (_vFds[i].fd == 0)
//					continue;
//				if ((_vFds[i].revents & POLLIN) == POLLIN){
//					//std::cout << "unexpected result, revent should be POLLIN" << std::endl;
//					if (_vFds[i].fd == _listenFd){///Listening socket is readable -> need to accept all incoming connections
//						try {
//							acceptNewConnect(i);
//						} catch (std::exception &e) {
//							//std::cerr << "failed with i = " << i << " and FD: " << _vFds[i].fd << "err message: " << e.what() << std::endl;
//						}
//											}
//					else { ///connection is not on listening socket, need to be readable -> receive all data
//						try {
//							recvConnection(i);
//						} catch (std::exception &e) {
//							//std::cerr << "failed with i = " << i << " and FD: " << _vFds[i].fd << "err message: " << e.what() << std::endl;
//						}
//
//
//					}
//				}
//			}
//
//		}
//	}//end of while loop
//
//}


































//void	Socket::sendData(int client_socket){
//
//	std::string response_header = "HTTP/1.0 200 OK\r\n"
//						   "Content-Type: text/html\r\n"
//						   "Content-Length: 348\r\n" // !! need to be extra careful with numbers content-lenght
//	"\r\n";
//
//	std::string filename = "/Users/yuliia/Codam/webserv/info_practice.html"; //HARDCODED file path
//	std::ifstream file(filename);
//	std::string response_body((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
//	std::cout << response_body << std::endl;
//
//	//header
//	int bitesend1 = (int)send(client_socket, response_header.c_str(), response_header.length(), 0);
//	if (bitesend1 < 0){
//		throw std::runtime_error("Socket : send");
//	}
//	std::cout << "header : bite sent: " <<bitesend1 << " response lenght: " << response_header.length() <<std::endl;
//
//	//body from file
//	int bitesend2 = (int)send(client_socket, response_body.c_str(), response_body.length(), 0);
//	if (bitesend2 < 0){
//		throw std::runtime_error("Socket : send");
//	}
//	std::cout << "body : bite sent: " <<bitesend2 << " response lenght: " << response_body.length() <<std::endl;
//}
