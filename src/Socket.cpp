//
//  Socket.cpp
//  server_xcode_project
//
//  Created by Julia Demura on 15/01/2023.
//

#include "Socket.hpp"
#include "methods.hpp"

int		Socket::getSocketFd(){return _listenFd;};
addrinfo * Socket::getAddrInfo(){return _addrinfo;};
std::vector<pollfd> &Socket::getPollFdVector(){return _vFds;};
size_t	Socket::getPollFdVectorSize(){return _vFds.size();};
void	Socket::setServers(std::map<std::string, std::vector<Server> > &servers){ _servers = &servers;};
	
std::vector<pollfd> &Socket::getCGIVector(int i){
	//std::cout << "getCGIVector for i= "<< i << std::endl;
	return _clients[i].cgiInfo.vCGI;
};

size_t	Socket::getCGIVectorSize(int i){
	//std::cout << "getCGIVectorSize for i= "<< i << " : " << _clients[i].cgiInfo.vCGI.size() << std::endl;
	return _clients[i].cgiInfo.vCGI.size();
};

bool	Socket::getCGIbool(int i){
	//std::cout << "getCGIbool for i= "<< i << std::endl;
	if (_vFdsSize < i){
		return false;
	}
	if (!_clients[i].isCGI){
		return false;
	}
	return _clients[i].isCGI;
};
void Socket::setPollFdVector(std::vector<pollfd> &vFds) {
    // _vFds = std::move(vFds);
	_vFds.clear();
	size_t i=0;
	for (std::vector<pollfd>::iterator it = vFds.begin(); it != vFds.end(); it++){
		_vFds[i].fd = it->fd;
		_vFds[i].events = it->events;
		_vFds[i].revents = it->revents;
		i++;
	}
	_vFdsSize = i;
	std::cout << "setPollFdVector size: " << _vFds.size() << std::endl;
}

void Socket::setPollFdVectorSize(size_t size) { _vFdsSize = size; }
	
void Socket::setPollFdVector1(int clientInd, const std::vector<pollfd>& pollFdVec){
	// _vFds[clientInd] = pollFdVec;
	_vFds = std::move(pollFdVec);

};

size_t Socket::numberOfConnections(){

	//std::cout << "numberOfConnections for i= "<< i << std::endl;
	size_t totalCguFdSize = 0;
	int clientIndex = 0;
	for (std::vector<pollfd>::iterator it = _vFds.begin(); it != _vFds.end(); it++){
			
		if (_clients[clientIndex].isCGI){
			totalCguFdSize += _clients[clientIndex].cgiInfo.vCGI.size();
		}
		clientIndex++;
	}
	return totalCguFdSize + _vFds.size();
};

void Socket::setCGIVector(int clientInd, const std::vector<pollfd>& cgiFdVec) {

    CGIInfo &cgiInfo = _clients[clientInd].cgiInfo;
    if (cgiInfo.vCGI.size() == 1) {
        cgiInfo.vCGI[0] = std::move(cgiFdVec[0]);
    } else if (cgiInfo.vCGI.size() == 2) {
        cgiInfo.vCGI[0] = std::move(cgiFdVec[0]);
        cgiInfo.vCGI[1] = std::move(cgiFdVec[1]);
   	}
};

void Socket::unpackVectorintoSocket(size_t &allCounter, size_t fdCounter, std::vector<pollfd> socketsAll) {

	CGIInfo &CGIinfo = _clients[fdCounter].cgiInfo;
	bool isCGI = _clients[fdCounter].isCGI;

	_vFds[fdCounter] = std::move(socketsAll[allCounter]);
	allCounter++;
	if(isCGI){
		//std::cout << "isCGI" << std::endl;
		//std::cout << "FD is = "<< socketsAll[allCounter].fd << std::endl;
		if (CGIinfo.vCGI.size() == 1) {

			CGIinfo.vCGI[0].fd = socketsAll[allCounter].fd;
			CGIinfo.vCGI[0].events = socketsAll[allCounter].events;
			CGIinfo.vCGI[0].revents = socketsAll[allCounter].revents;

			//CGIinfo.vCGI[0] = std::move(socketsAll[allCounter]);
			allCounter++;
		} else if (CGIinfo.vCGI.size() == 2) {
			CGIinfo.vCGI[0].fd = socketsAll[allCounter].fd;
			CGIinfo.vCGI[0].events = socketsAll[allCounter].events;
			CGIinfo.vCGI[0].revents = socketsAll[allCounter].revents;
			//CGIinfo.vCGI[0] = std::move(socketsAll[allCounter]);
			allCounter++;
			CGIinfo.vCGI[1].fd = socketsAll[allCounter].fd;
			CGIinfo.vCGI[1].events = socketsAll[allCounter].events;
			CGIinfo.vCGI[1].revents = socketsAll[allCounter].revents;
			//CGIinfo.vCGI[1] = std::move(socketsAll[allCounter]);
			allCounter++;
		}
		//std::cout << "CGIinfo.vCGI[0].fd =" << CGIinfo.vCGI[0].fd << std::endl;
		//std::cout << "CGIinfo.vCGI[1].fd =" << CGIinfo.vCGI[1].fd << std::endl;
	}
		
};

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

	ClientInfo clientStruct;
	_clients.push_back(clientStruct);
	_clients[0].recvBytes = 0;
	_clients[0].receivedContent.clear();
	_clients[0].reply.clear();
	_clients[0].biteToSend = 0;
	_clients[0].isCGI = false;
	_clients[0].CgiDone = false;
	_clients[0].cgiInfo.vCGI.clear();
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
		if (contentLen != 0) {
			if (contentLen + headrSize > recvBites){
				return false;
			}
			else if (recvBites == headrSize + contentLen){
				return true;
			}
		} else {
			if (res < MAX_REQUEST_SIZE){
				return true;
			}
		}
	}
	return false;
}

void	Socket::closeClientConnection(int i){

	close(_vFds[i].fd);
	_vFds.erase(_vFds.begin() + i);
	_vFdsSize--;
	_clients.erase(_clients.begin() + i);
}

void	Socket::sendData(int i){
	std::cout << "------sendData starts------" << std::endl;
	std::cout << "fd I have: " << _vFds[0].fd << " , " << _vFds[1].fd << " , " << _vFds[2].fd << std::endl;
	std::cout << "fd I want to send: " << _vFds[i].fd << std::endl;
	std::cout << _clients[i].reply << std::endl;
	std::cout << "---------------------------" << std::endl;

	size_t bitesend = send(_vFds[i].fd, _clients[i].reply.c_str(), _clients[i].reply.length(), 0);
	if (bitesend < 0){
		throw std::runtime_error("Socket : send");
	} else if (bitesend < _clients[i].biteToSend){
		std::string rest = _clients[i].reply.substr(bitesend, _clients[i].reply.length());
		_clients[i].reply = rest;
		_clients[i].biteToSend -= bitesend;
		//std::cout << "bites to send left: " << _clients[i].biteToSend << std::endl;
	} else if (_clients[i].biteToSend == bitesend){
		//std::cout << "sendData ends" << std::endl;
		if (_clients[i].isCGI == true && _clients[i].CgiDone == true) {
			std::cout << "_clients[i].isCGI == true && _clients[i].CgiDone == true | all sent" << std::endl;
			_clients[i].isCGI = false;
			_clients[i].CgiDone = false;
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
		_vFdsSize = _vFds.size();
		
		//initiate client.struct
		ClientInfo clientStruct;
		clientStruct.biteToSend = 0;
		clientStruct.recvBytes = 0;
		clientStruct.receivedContent.clear();
		clientStruct.reply.clear();
		clientStruct.isCGI = false;
		_clients.push_back(clientStruct); //erase it when time comes UNFINISHED
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
				if (_clients[i].isCGI == false){

					_vFds[i].events |= POLLOUT;
					//std::cout << "no CGI" << std::endl;
				}
				else {
					_clients[i].cgiInfo.state = NO_PIPES;
					_clients[i].CgiDone = false;
					_clients[i].cgiInfo.childPid = 0;

					// std::cout << "---- CGI request struct ----" << std::endl;
					// std::cout << "method: " << _clients[i].ClientRequest.method << std::endl;
					// std::cout << "body: " << _clients[i].ClientRequest.requestBody << std::endl;
					// std::cout << "bodyLen: " << _clients[i].ClientRequest.requestBodyLen << std::endl;
					// std::cout << "---------------------------" << std::endl;
				}
			} catch (std::exception &e) {
				std::cerr << "Caught exception: " << e.what() << std::endl;
			}
		}
	}
}

void	Socket::CGIerrorReply(int i){

	ClientInfo &client = _clients[i];

	std::string hostPort = _hostName + ":" + _portNumber;
	client.ClientRequest.code = 500;
	client.ClientResponse = responseStructConstruct(*_servers, hostPort, "", client.ClientRequest);
	client.reply = formResponseString(client.ClientResponse);
	client.CgiDone = true;
	_vFds[i].events |= POLLOUT;
	// if (client.cgiInfo.envp != NULL){
	// 	freeEnvp(client.cgiInfo.envp);
	// 	closePipes(client.cgiInfo.pipeFdIn, client.cgiInfo.pipeFdOut);
	// }
	client.isCGI = false;
	client.cgiInfo.vCGI.clear();
	client.cgiInfo.vCGIsize = 0;
}

void	Socket::readFromChild(int i){

	//std::cout << "========read from child========" << std::endl;
	try{
		readChild(_clients[i].cgiInfo.pipeFdOut, _clients[i].reply);
		//std::cout << "reply from child: " << _clients[i].reply << std::endl;
	}
	catch (std::exception &e) {
		std::cerr << "Failed to read from child: " << e.what() << std::endl;
		_clients[i].cgiInfo.state = ERROR; 
		CGIerrorReply(i);
		return ;
	}
	//std::cout << "========read child done========" << std::endl;
}

void	Socket::writeInChild(int i){

	ClientInfo &client = _clients[i];
	CGIInfo &cgiInf = client.cgiInfo;

	//std::cout << "========writing in child========" << std::endl;
	try{
		// std::cout << "body:" << std::endl;
		// std::cout << _clients[i].ClientRequest.requestBody << std::endl;
		// std::cout << "body end " << std::endl;

		size_t wrote = writeChild(client.ClientRequest.requestBody.c_str(), client.ClientRequest.requestBody.length(), cgiInf.pipeFdIn);
		client.ClientRequest.requestBody.erase(0, wrote);
		//std::cout << "wrote: " << wrote << std::endl;
		if (client.ClientRequest.requestBody.empty() || wrote == 0){
			cgiInf.state = WRITE_DONE;
			cgiInf.vCGI.erase(cgiInf.vCGI.begin()); //write is in vCGI[0]
			cgiInf.vCGIsize = 1;
			//std::cout << "write done, vCGi size is " << cgiInf.vCGI.size() <<  std::endl;
		}
	}
	catch (std::exception &e) {
		std::cerr << "Failed to write in child: " << e.what() << std::endl;
		_clients[i].cgiInfo.state = ERROR;
		CGIerrorReply(i);
		return ;
	}
}

void	Socket::startChild(int i){
	ClientInfo &client = _clients[i];
	CGIInfo &cgiInf = client.cgiInfo;
	try{
		cgiInf.childPid = launchChild(cgiInf, client.ClientRequest, _portNumber, _hostName);
		cgiInf.state = PIPES_INIT;
		cgiInf.vCGIsize = 2;
		cgiInf.childExited = false;
	} catch (std::exception &e) { 
		std::cerr << "Failed to init pipes: " << e.what() << std::endl;
		cgiInf.state = ERROR;
		CGIerrorReply(i);
	}
}

void Socket::pickCGIState(int i){

	CGIInfo &cgiInf = _clients[i].cgiInfo;
	pollfd &writeFd = cgiInf.vCGI[0]; //expect POLLOUT
	pollfd &readFd = cgiInf.vCGI[1]; //expect POLLIN

	if (cgiInf.state == NO_PIPES){
		std::cout << "no pipes yet" << std::endl;
	}
	else if (cgiInf.state == PIPES_INIT && (writeFd.revents & POLLOUT) == POLLOUT){
		cgiInf.state = WRITE_READY;
		std::cout << "write ready, vCGi size is " << cgiInf.vCGI.size() <<  std::endl;
	}
	if (cgiInf.state == WRITE_DONE){
		try {
			int stChild;
			waitChild(stChild, cgiInf.childPid, cgiInf.childExited);
			//std::cout << stChild << " | " << cgiInf.childExited << " | " << cgiInf.childPid<< std::endl;
			if (cgiInf.childExited == true){ // && stChild > 0
				//cgiInf.state = READ_READY;
				readFd = cgiInf.vCGI[0];
				//std::cout << "read ready, vCGi size is " << cgiInf.vCGI.size() <<  std::endl;
			}
		} catch (std::exception &e) {
			std::cerr << "Failed to wait for child: " << e.what() << std::endl;
			cgiInf.state = ERROR;
			CGIerrorReply(i);
			return ;
		}
	}
	if (cgiInf.state == WRITE_DONE && (readFd.revents & POLLIN)== POLLIN){ // && (readFd.revents & POLLIN)== POLLIN)
		cgiInf.state = READ_READY;
		std::cout << "READ_READY, vCGi size is " << cgiInf.vCGI.size() <<  std::endl;
	}
	else if (cgiInf.state == READ_READY && (readFd.revents & POLLHUP)== POLLHUP){
		cgiInf.state = READ_DONE;
		cgiInf.vCGI.erase(cgiInf.vCGI.begin()); //read in vCGI[1] but write should be erased already NOT SURE
		cgiInf.vCGIsize = 0;
		std::cout << "READ_DONE, vCGi size is expected 0 and is: " << cgiInf.vCGI.size() <<  std::endl;
	}
}

void	Socket::checkCGIevens(int i){ 

	if (_clients[i].CgiDone == true)
		return ;

	pickCGIState(i);
	if (_clients[i].cgiInfo.state == NO_PIPES){ 
		startChild(i);

	} else if (_clients[i].cgiInfo.state == WRITE_READY){
		writeInChild(i);
		
	} else if (_clients[i].cgiInfo.state == READ_READY){ //read from child
		readFromChild(i);

	} else if (_clients[i].cgiInfo.state == READ_DONE){ //read done, close pipes, wait for child
		//std::cout << "------- //read done, close pipes, wait for child -------" << std::endl;
		parseCorrectResponseCGI(_clients[i].reply, _clients[i].ClientResponse);
		_clients[i].CgiDone = true;
		//freeEnvp(_clients[i].cgiInfo.envp);
		//closePipes(_clients[i].cgiInfo.pipeFdIn, _clients[i].cgiInfo.pipeFdOut);
		_vFds[i].events |= POLLOUT; // &= ~POLLOUT;
	}
}

void	Socket::checkEvents(){
	
	for (int i = 0; i < (int)_vFds.size(); i++){
		if ((_vFds[i].revents & POLLIN) == POLLIN){
			if (_vFds[i].fd == _listenFd){
				try {
					acceptNewConnect(i);
				} catch (std::exception &e) {
					std::cerr << "failed with i = " << i << " and FD: " << _vFds[i].fd << "err message: " << e.what() << std::endl;
				}
			} else {
				try {
					recvConnection(i);
				} catch (std::exception &e) {
					std::cerr << "failed with i = " << i << " and FD: " << _vFds[i].fd << "err message: " << e.what() << std::endl;
				}
			}
		} else if ((_vFds[i].revents & POLLOUT) == POLLOUT){
			sendData(i);
			
		} else if ((_vFds[i].revents & POLLHUP) == POLLHUP){
			try {
				std::cerr << "close connection" << std::endl;
				closeClientConnection(i);
			} catch (std::exception &e) {
				std::cerr << "failed to close client with FD: " << _vFds[i].fd << " err message: " << e.what() << std::endl;
			}
		}

		if (i > 0 && _clients[i].isCGI == true){
			checkCGIevens(i);
		}
	}
	
}
