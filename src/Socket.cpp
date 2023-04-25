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
		_clients.push_back(clientStruct);
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
				_clients[i].ClientRequest.parsBuff = _clients[i].receivedContent;
				_clients[i].reply = methods(_clients[i].ClientRequest, *_servers, _portNumber, _hostName, _clients[i].isCGI);
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

//there might be more vCGI then 1, as a 
//few clients from this server can call CGI. I can fix issue with indexes as I have index i that shows the client. 
//for every client there are 2 pipes, so I can use i*2 and i*2+1 to get the right pipes. 
// this need to be fixed on Socket - poll loop level.

void	Socket::CGIerrorReply(int i){

	std::string hostPort = _hostName + ":" + _portNumber;
	_clients[i].ClientRequest.code = 500;
	_clients[i].ClientResponse = responseStructConstruct(*_servers, hostPort, "", _clients[i].ClientRequest);
	_clients[i].reply = formResponseString(_clients[i].ClientResponse);
	_clients[i].CgiDone = true;
	_vFds[i].events |= POLLOUT;
	if (_clients[i].cgiInfo.envp != NULL){
		freeEnvp(_clients[i].cgiInfo.envp);
		closePipes(_clients[i].cgiInfo.pipeFdIn, _clients[i].cgiInfo.pipeFdOut);
	}
		
}

void	Socket::checkCGIevens(int i){ 

	int j = i * 2;

	std::cout << "-------checkCGIevens-------" << std::endl;

	if (_vCGISize == 0 || _clients[i].isCGI == false)
		return ;
	if (_clients[i].CgiDone == true)
		return ;

	else if (_vCGISize == 0 && _clients[i].isCGI == true){ //init pipes and create child // NOT the size of vCGI but other check for first launch of this function
		
		try{
			std::cout << "launching child" << std::endl;
			_clients[i].cgiInfo.childPid = launchChild(_clients[i].cgiInfo, _clients[i].ClientRequest, _portNumber, _hostName);
			_vCGISize += 2;
			_vCGI[j + 1].events |= POLLOUT;
		} catch (std::exception &e) { 
			std::cerr << "Failed to init pipes: " << e.what() << std::endl;
			CGIerrorReply(i);
			//free+close is done in launchChild
			std::cout << "------- end of checkCGIevens : launching child -------" << std::endl;
			return ;
		}
		
	} else if ((_vCGI[j + 1].revents & POLLOUT) == POLLOUT){ //write in child, wait for child
		try{
			std::cout << "writing in child" << std::endl;
			writeInChild(_clients[i].ClientRequest.requestBody.c_str(), _clients[i].ClientRequest.requestBody.length() , _clients[i].cgiInfo.pipeFdIn);
		}
		catch (std::exception &e) {
			std::cerr << "Failed to write in child: " << e.what() << std::endl;
			CGIerrorReply(i);
			return ;
		}
		
		try{
			std::cout << "waiting for child" << std::endl;
			_clients[i].cgiInfo.statusChild = 0;
			waitForChild(_clients[i].cgiInfo.statusChild, _clients[i].cgiInfo.childPid);
			std::cout << "waiting for child done, status : " << _clients[i].cgiInfo.statusChild << std::endl;
		}
		catch (std::exception &e) {
			std::cerr << "Caught exception: " << e.what() << std::endl;
			CGIerrorReply(i);
			return ;
		}
		if (_clients[i].cgiInfo.statusChild < 0){ //not sure if to check that here
			std::cerr << "error in child : if (statusChild < 0) | from POLLOUT" <<std::endl; 
			CGIerrorReply(i);
			return ;
		} else {
			_vCGI[j].events |= POLLIN;
			std::cout << "------- end of checkCGIevens : waiting done child -------" << std::endl;
			return ;
		}


	} else if ((_vCGI[j].revents & POLLIN)== POLLIN){ //read from child
		
		if (_clients[i].cgiInfo.statusChild < 0){ //not sure if to check that here
			std::cerr << "error in child : if statusChild < 0 | from POLLIN" << std::endl; 
			CGIerrorReply(i);
			return ;
		}

		try{
			std::cout << "reading from child" << std::endl;
			readFromChild(_clients[i].cgiInfo.pipeFdOut, _clients[i].reply);
			std::cout << "reply from child: " << _clients[i].reply << std::endl;
			parseCorrectResponseCGI(_clients[i].reply, _clients[i].ClientResponse);
			_clients[i].CgiDone = true;
			freeEnvp(_clients[i].cgiInfo.envp);
			closePipes(_clients[i].cgiInfo.pipeFdIn, _clients[i].cgiInfo.pipeFdOut);
		}
		catch (std::exception &e) {
			std::cerr << "Failed to read from child: " << e.what() << std::endl;
			CGIerrorReply(i);
			return ;
		}
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
		if (i > 0 && _clients[i].isCGI == true){
			std::cout << "CGI events | i = " << i << std::endl;
			checkCGIevens(i);
		}
	}
	
}
