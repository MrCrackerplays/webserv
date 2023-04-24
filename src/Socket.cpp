//
//  Socket.cpp
//  server_xcode_project
//
//  Created by Julia Demura on 15/01/2023.
//

#include "Socket.hpp"
#include "methods.hpp"

void	printVectPollFd(std::vector<pollfd> vect){
	
	for(std::vector<pollfd>::iterator it = vect.begin(); it != vect.end(); it++){
		pollfd local = *it;
		std::cout << local.fd << std::endl;
	}
}

void	printVectStr(std::vector<std::string> vect){
	
	for(std::vector<std::string>::iterator it = vect.begin(); it != vect.end(); it++){
		std::cout << *it << std::endl;
	}
}

int		Socket::getSocketFd(){
	return _listenFd;
}

addrinfo *	Socket::getAddrInfo(){
	return _addrinfo;
}

std::vector<pollfd> &Socket::getPollFdVector(){
	return _vFds;
}

std::vector<pollfd> &Socket::getCGIVector(){
	return _vCGI;
}

size_t	Socket::getPollFdVectorSize(){
	return _vFds.size();
}

size_t	Socket::getCGIVectorSize(){
	return _vCGI.size();
}

void	Socket::setServers(std::map<std::string, std::vector<Server> > &servers){
	
	_servers = &servers;
	
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
	//freeaddrinfo(_addrinfo); //pointer being freed after allocating with new - check needed
}

size_t	findContentLenght(std::string buffer){
	
	size_t pos = buffer.find("Content-Length: ");
	if (pos != std::string::npos){
		pos += std::strlen("Content-Length: ");
		size_t endpos = buffer.find("\r\n", pos);
		if (endpos != std::string::npos){
			return std::stoi(buffer.substr(pos, endpos - pos));
		}
	}
	return 0;
}

bool	fullRequestReceived(std::string buffer, size_t recvBites, size_t res){

	size_t contentLen = 0;
	size_t headrSize = buffer.find("\r\n\r\n");
	if (headrSize != std::string::npos){
		headrSize += 4;
		contentLen = findContentLenght(buffer);
		//std::cout << n << " "  << recvBites << " " << headrSize << std::endl;
		if (contentLen != 0) {
			if (contentLen + headrSize > recvBites){
				//std::cout << "fullRequestReceived - false" << std::endl;
				return false;
			}
			else if (recvBites == headrSize + contentLen){
				//std::cout << "fullRequestReceived - true" << std::endl;
				return true;
			}
		} else {
			if (res < MAX_REQUEST_SIZE){
				//std::cout << "fullRequestReceived - true" << std::endl;
				return true;
			}
		}
	}
	//std::cout << "fullRequestReceived - false" << std::endl;
	return false;
}

void	Socket::closeClientConnection(int i){

	close(_vFds[i].fd);
	_vFds.erase(_vFds.begin() + i);
	//_clients[i].recvBytes = 0;
	//_clients[i].receivedContent.clear();
	_clients.erase(_clients.begin() + i);
}

void	Socket::sendData(int i){
		
	size_t bitesend = send(_vFds[i].fd, _clients[i].reply.c_str(), _clients[i].reply.length(), 0);
	if (bitesend < 0){
		throw std::runtime_error("Socket : send");
	} else if (bitesend < _clients[i].biteToSend){
		//not all was sent at once
		std::string rest = _clients[i].reply.substr(bitesend, _clients[i].reply.length());
		_clients[i].reply = rest;
		_clients[i].biteToSend -= bitesend;
	} else if (_clients[i].biteToSend == bitesend){
		//all sent
		try {
			closeClientConnection(i);
		} catch (std::exception &e) {
			std::cerr << "failed to close client with FD: " << _vFds[i].fd << " err message: " << e.what() << std::endl;
		}
	}
}

//work with listening socket
void	Socket::acceptNewConnect(int i){
	
	for (int j = i+1; ; j++) {
		int newFd = accept(_listenFd, NULL, NULL);
		if (newFd < 0) {
			if (errno != EWOULDBLOCK) {
				perror("");
				throw std::runtime_error("SocketLoop : accept");
			}
			break;
		}
		pollfd newPollfd;
		newPollfd.fd = newFd;
		newPollfd.events = POLLIN | POLLHUP; //added POLLHUP though it may be not proper place to add both
		_vFds.push_back(newPollfd);
		if (i == 0 && _clients.size() == 0){
			ClientInfo clientStruct;
			_clients.push_back(clientStruct);
		}

		//initiate client.struct
		ClientInfo clientStruct;
		clientStruct.biteToSend = 0;
		clientStruct.recvBytes = 0;
		clientStruct.receivedContent.clear();
		clientStruct.reply.clear();
		clientStruct.isCGI = false;
		clientStruct.code = 200;
		clientStruct.pipeFdIn = 0;
		clientStruct.pipeFdOut = 0;
		_clients.push_back(clientStruct);
//		if (_clients.size() >= j){
//			_clients[j].sd = newFd;//check if j can be actually bigger then 0 first.
//		} else {
//			std::cout << "donnow what to do with this yet, j == " << j << std::endl;
//		}
		
	}
}

void	Socket::recvConnection(int i){
	
	int res = 0;
	char buff[MAX_REQUEST_SIZE];
	std::string buffer;
	//std::cout << _vFds[i].fd << std::endl;
	
	res = (int)recv(_vFds[i].fd, buff, MAX_REQUEST_SIZE - 1, 0);
	// std::cout << "================================" << std::endl;
	// std::cout << "bites read : " << res << std::endl;
	// buff[res] = '\0';
	// std::cout << buff << std::endl;
	// std::cout << "================================" << std::endl;
	if (res == -1){
		return;
	} else if (res < 0){
		closeClientConnection(i);
		std::cerr << "recv failed on i = " << i << "FD: " << _vFds[i].fd << strerror(errno) << std::endl;
		throw std::runtime_error("SockedLoop : recv");
	} else if (res == 0){
		std::cerr << "connection was closed by client   " << "for fd " << _vFds[i].fd << std::endl; //unfinished, check for POLLHUP
		closeClientConnection(i);
	} else {
		//add on to buffer
		buff[res] = '\0';
		_clients[i].recvBytes += res;
		_clients[i].receivedContent.append(buff, res);
		if (fullRequestReceived(_clients[i].receivedContent, _clients[i].recvBytes, res)){
			//parsing part
			try {
				_clients[i].reply = methods(_clients[i].receivedContent, *_servers, _portNumber, _hostName, _clients[i].isCGI);
				_clients[i].biteToSend = _clients[i].reply.length();
				_vFds[i].events |= POLLOUT;
				//std::cout << "revent: " << _vFds[i].revents << std::endl;
			} catch (std::exception &e) {
				std::cerr << "Caught exception: " << e.what() << std::endl;
				//std::cerr << "PROBABLY should catch it in methods" << std::endl;
			}

		}
	}
}

void	Socket::checkCGIevens(int i){

	if (_vCGISize == 0 && _clients[i].isCGI == false)
		return ;
	else if (_vCGISize == 0 && _clients[i].isCGI == true){
		//init pipes and create child
		try{
			initPipesCreatePollFDstruct(_vCGI, _clients[i].pipeFdIn, _clients[i].pipeFdOut);
			_clients[i].childPid = fork();
			envpGenerateNew(_clients[i].envp, _clients[i].receivedContent, _clients[i].recvBytes);
		} catch (std::exception &e) { 
			freeEnvp(_clients[i].envp);
			_clients[i].code = 500; //server error
			std::cerr << "Failed to init pipes: " << e.what() << std::endl;
			return ;
		}
		_vCGISize = 2;
	} else if ((_vCGI[1].revents & POLLOUT) == POLLOUT){
		//write into child
		//write(pipeFdIn[1]) ->> create a child to write into it





	} else if ((_vCGI[0].revents & POLLIN)== POLLIN){
		//read(pipeFdOut[0])
		//read from child
	}



	

}

void	Socket::checkEvents(){
	for (int i = 0; i < (int)_vFds.size(); i++){
		if ((_vFds[i].revents & POLLIN) == POLLIN){
			
			//listening socket event
			if (_vFds[i].fd == _listenFd){
				try {
					//std::cout << "try to accept new connection" << std::endl;
					acceptNewConnect(i);
					//std::cout << "end of accept new connection" << std::endl;
				} catch (std::exception &e) {
					std::cerr << "failed with i = " << i << " and FD: " << _vFds[i].fd << "err message: " << e.what() << std::endl;
				}
			//client's from listening socket event
			} else {
				try {
					//std::cout << "try to receive" << std::endl;
					recvConnection(i);
					//std::cout << "end of receive" << std::endl;
				} catch (std::exception &e) {
					std::cerr << "failed with i = " << i << " and FD: " << _vFds[i].fd << "err message: " << e.what() << std::endl;
				}
			}
		} else if ((_vFds[i].revents & POLLOUT) == POLLOUT){
			//std::cout << "try to send data" << std::endl;
			sendData(i);
			//std::cout << "end send data" << std::endl;
			
		} else if ((_vFds[i].revents & POLLHUP) == POLLHUP){
			//std::cout << "pollHUP" << std::endl;
			try {
				std::cerr << "close connection" << std::endl;
				closeClientConnection(i);
			} catch (std::exception &e) {
				std::cerr << "failed to close client with FD: " << _vFds[i].fd << " err message: " << e.what() << std::endl;
			}
		
		}
	}
}
