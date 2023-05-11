//
//  Socket.cpp
//  server_xcode_project
//
//  Created by Julia Demura on 15/01/2023.
//

#include "Socket.hpp"
#include "methods.hpp"

int		Socket::getSocketFd(){return _listenFd;};
struct addrinfo * Socket::getAddrInfo(){return _addrinfo;};
std::vector<struct pollfd> &Socket::getPollFdVector(){return _vFds;};
size_t	Socket::getPollFdVectorSize(){return _vFds.size();};
void	Socket::setServers(std::map<std::string, std::vector<Server> > &servers){ _servers = &servers;};
	
std::vector<struct pollfd> &Socket::getCGIVector(int i){
	// if (_clients.size() < i)
	// 	return _clients[0].cgiInfo.vCGI;
	return _clients[i].cgiInfo.vCGI;
};

size_t	Socket::getCGIVectorSize(int i){
	return _clients[i].cgiInfo.vCGI.size();
};

bool	Socket::getCGIbool(int i){
	// if (_vFdsSize < i){
	// 	return false;
	// }
	// if (!_clients[i].isCGI){
	// 	//std::cout << "getCGIbool: _clients[i].isCGI is FALSE" << std::endl;
	// 	return false;
	// }
	//std::cout << "getCGIbool: _clients[i].isCGI is TRUE" << std::endl;
	if (_clients[i].CgiDone)
		return false;
	
	if (_clients[i].isCGI == true)
		return true;
	return false;
};

void Socket::setPollFdVectorSize(size_t size) { _vFdsSize = size; }

size_t Socket::numberOfConnections(){

	size_t totalCguFdSize = 0;
	size_t clientIndex = 0;
	size_t clientvectSize = _clients.size();
	for (std::vector<struct pollfd>::iterator it = _vFds.begin(); it != _vFds.end(); it++){
		if (clientIndex > clientvectSize){
			break;
		}
		else if (_clients[clientIndex].isCGI){
			totalCguFdSize += _clients[clientIndex].cgiInfo.vCGI.size();
		}
		clientIndex++;
	}
	return totalCguFdSize + _vFds.size();
};

void Socket::unpackVectorintoSocket(size_t &allCounter, size_t fdCounter, std::vector<struct pollfd> &socketsAll) {

	if (_vFds.size() < fdCounter){
		//std::cout << "_vFds.size() < fdCounter but this should not happen" << std::endl;
		return;
	}
	_vFds[fdCounter].fd = socketsAll[allCounter].fd;
	_vFds[fdCounter].events = socketsAll[allCounter].events;
	_vFds[fdCounter].revents = socketsAll[allCounter].revents;
	allCounter++;

	if (_clients.size() > fdCounter && _clients[fdCounter].isCGI){
		CGIInfo &CGIinfo = _clients[fdCounter].cgiInfo;
			if (CGIinfo.vCGI.size() == 1) {
				if (socketsAll.size() < allCounter + 1){
					//std::cout << "socketsAll.size() < allCounter" << std::endl;
					return;
				}
				CGIinfo.vCGI[0].fd = socketsAll[allCounter].fd;
				CGIinfo.vCGI[0].events = socketsAll[allCounter].events;
				CGIinfo.vCGI[0].revents = socketsAll[allCounter].revents;
				allCounter++;
			} else if (CGIinfo.vCGI.size() == 2) {
				if (socketsAll.size() < allCounter + 2){
					return;
				}
				CGIinfo.vCGI[0].fd = socketsAll[allCounter].fd;
				CGIinfo.vCGI[0].events = socketsAll[allCounter].events;
				CGIinfo.vCGI[0].revents = socketsAll[allCounter].revents;
				allCounter++;
				CGIinfo.vCGI[1].fd = socketsAll[allCounter].fd;
				CGIinfo.vCGI[1].events = socketsAll[allCounter].events;
				CGIinfo.vCGI[1].revents = socketsAll[allCounter].revents;
				allCounter++;
			}
				
	}
	
};

void	printVectPollFd(std::vector<struct pollfd> vect){
	
	for(std::vector<struct pollfd>::iterator it = vect.begin(); it != vect.end(); it++){
		struct pollfd local = *it;
		std::cout << local.fd << std::endl;
	}
}

void	printVectStr(std::vector<std::string> vect){
	
	for(std::vector<std::string>::iterator it = vect.begin(); it != vect.end(); it++){
		std::cout << *it << std::endl;
	}
}

void	Socket::createAddrinfo(){

	int status;
	struct addrinfo hints;
	
	std::memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	
	status = getaddrinfo(_hostName.c_str(), _portNumber.c_str(), &hints, &_addrinfo);
	if (status < 0){
		throw std::runtime_error("Port : getaddrinfo");
	}
	_listenFd = socket(PF_INET, SOCK_STREAM, 0);
	if (_listenFd < 0){
		throw std::runtime_error("Socket : socket");
	}
}

Socket::Socket(char * hostName, char * portNumber){
	
	_addrinfo = NULL;

	_portNumber = portNumber;
	_hostName = hostName;
	_listenFd = 0;

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
	if (_addrinfo) {
		freeaddrinfo(_addrinfo);
	}
	if (_listenFd > 0){
		close(_listenFd);
	}
}

Socket::Socket(const Socket &src) {
	*this = src;
}

Socket& Socket::operator=(const Socket &src) {
	if (this == &src)
		return *this;
	this->_addrinfo = src._addrinfo;
	this->_clients = src._clients;
	this->_hostName = src._hostName;
	this->_listenFd = src._listenFd;
	this->_portNumber = src._portNumber;
	this->_servers = src._servers;
	this->_vFds = src._vFds;
	this->_vFdsSize = src._vFdsSize;
	return *this;
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
		struct pollfd newPollfd;
		newPollfd.fd = newFd;
		newPollfd.events = POLLIN | POLLHUP; //added POLLHUP though it may be not proper place to add both
		_vFds.push_back(newPollfd);
		_vFdsSize = _vFds.size();

		//initiate client.struct
		ClientInfo clientStruct;
		clientStruct.biteToSend = 0;
		clientStruct.biteSentInt = 0;
		clientStruct.recvBytes = 0;
		clientStruct.receivedContent.clear();
		clientStruct.receivedContentVector.clear();
		clientStruct.reply.clear();
		clientStruct.isCGI = false;
		clientStruct.startTime = std::time(nullptr);
		clientStruct.timerOn = true;
		clientStruct.timeout = false;
		clientStruct.ClientRequest.code = 0;
		_clients.push_back(clientStruct);
	}
}

void transferBufferToVector(const char* buff, ssize_t buffSize, std::vector<char>& rowData) {
   	rowData.insert(rowData.end(), buff, buff + buffSize);
}

void removeHeaderFromRowData(std::vector<char>& rowData) {

    const std::string doubleCRLF = "\r\n\r\n";
    std::string rowDataStr(rowData.begin(), rowData.end());
    size_t headerEndPos = rowDataStr.find(doubleCRLF);

    if (headerEndPos != std::string::npos) {
        size_t headerLength = headerEndPos + doubleCRLF.length();
        rowData.erase(rowData.begin(), rowData.begin() + headerLength);
    }
}

void	Socket::recvConnection(int i){
	ssize_t res = 0;
	char buff[MAX_REQUEST_SIZE];
	std::string buffer;
	
	res = recv(_vFds[i].fd, buff, MAX_REQUEST_SIZE - 1, 0);
	if (res == -1){
		return;
	} else if (res < 0){
		closeClientConnection(i);
		throw std::runtime_error("SockedLoop : recv");
	} else if (res > 0) {
		transferBufferToVector(buff, res, _clients[i].receivedContentVector);
		buff[res] = '\0';
		_clients[i].recvBytes += res;
		_clients[i].receivedContent.append(buff, res);
		if (fullRequestReceived(_clients[i].receivedContent, _clients[i].recvBytes, res)){
			//parsing part
			try {
				_clients[i].timerOn = false;
				_clients[i].ClientRequest.parsBuff = _clients[i].receivedContent;
				_clients[i].reply = methods(_clients[i].ClientRequest, *_servers, _portNumber, _hostName, _clients[i].isCGI);
				if (_clients[i].isCGI == false){
					_vFds[i].events |= POLLOUT;
					_clients[i].biteToSend = _clients[i].reply.size();
				}
				else {
					removeHeaderFromRowData(_clients[i].receivedContentVector);
					_clients[i].cgiInfo.state = NO_PIPES;
					_clients[i].CgiDone = false;
					_clients[i].cgiInfo.childPid = 0;
					_clients[i].cgiInfo.contentLenghtCGI = _clients[i].receivedContentVector.size();
					
					if (_clients[i].cgiInfo.contentLenghtCGI > _clients[i].ClientRequest.allowedContLen){
						_clients[i].receivedContentVector.erase(_clients[i].receivedContentVector.begin() + _clients[i].ClientRequest.allowedContLen, _clients[i].receivedContentVector.end()); //maybe +1
						// std::cout << "contentLenghtCGI > allowedContLen, cgi body was adjusted" << std::endl; //CORRECT?
					}
				}
			} catch (std::exception &e) {
				std::cerr << "Caught exception: " << e.what() << std::endl;
			}
		}
	}
}

void	Socket::closeClientConnection(int i){
	close(_vFds[i].fd);
	_vFds.erase(_vFds.begin() + i, _vFds.begin() + i + 1);
	_vFdsSize--;
	_clients[i].receivedContent.clear();
	_clients[i].receivedContentVector.clear();
	_clients.erase(_clients.begin() + i, _clients.begin() + i + 1);
}

void	Socket::sendData(int i){

	ssize_t biteSent = send(_vFds[i].fd, _clients[i].reply.c_str(), _clients[i].reply.length(), 0);
	if (biteSent == -1){
		throw std::runtime_error("Socket : send");
	} else if (biteSent < _clients[i].biteToSend){
		std::string rest = _clients[i].reply.substr(biteSent, _clients[i].reply.length());
		_clients[i].reply = rest;
		_clients[i].biteToSend -= biteSent;
	} else if (_clients[i].biteToSend == biteSent){
		closeClientConnection(i);
		
	}
}

void	Socket::CGIerrorReply(int i){

	ClientInfo &client = _clients[i];

	std::string hostPort = _hostName + ":" + _portNumber;
	if (client.cgiInfo.state == TIMEOUT){
		client.ClientRequest.code = 408;
	} else {
		client.ClientRequest.code = 500;
	}
	client.ClientResponse = responseStructConstruct(*_servers, hostPort, "", client.ClientRequest);
	client.reply = formResponseString(client.ClientResponse);
	client.CgiDone = true;
	_vFds[i].events |= POLLOUT;
	//_clients[i].cgiInfo.state = ERROR;
	_clients[i].isCGI = false;

	if (client.cgiInfo.vCGI.size() == 2){
		close(client.cgiInfo.vCGI[0].fd);
		close(client.cgiInfo.vCGI[1].fd);
	} else if (client.cgiInfo.vCGI.size() == 1){
		close(client.cgiInfo.vCGI[0].fd);
	}
	client.cgiInfo.vCGI.clear();
	client.cgiInfo.vCGIsize = 0;
	client.CgiDone = true;

}

void	Socket::readFromChild(int i){

	try{
		readChild(_clients[i].cgiInfo.pipeFdOut, _clients[i].cgiReply);
	}
	catch (std::exception &e) {
		std::cerr << "Failed to read from child: " << e.what() << std::endl;
		_clients[i].cgiInfo.state = ERROR; 
		CGIerrorReply(i);
		return ;
	}
}

void	Socket::writeInChild(int i){

	ClientInfo &client = _clients[i];
	CGIInfo &cgiInf = client.cgiInfo;

	try{
		std::vector<char> &rowData = client.receivedContentVector;
		ssize_t wrote = writeChild(rowData, client.cgiInfo.offset, cgiInf.pipeFdIn);
	
		if (wrote == -1) {
    		return;
		}
		if (rowData.size() == client.cgiInfo.offset){
			cgiInf.state = WRITE_DONE;
			cgiInf.contentLenghtCGI = rowData.size();
			cgiInf.vCGI.erase(cgiInf.vCGI.begin()); //write is in vCGI[0]
			cgiInf.vCGIsize = 1;
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
		std::string hp = _hostName + ":" + _portNumber;
		cgiInf.childPid = launchChild(cgiInf, client.ClientRequest, _portNumber, _hostName, getServer(*_servers, hp, client.ClientRequest.hostNameHeader));
		cgiInf.state = PIPES_INIT;
		cgiInf.vCGIsize = 2;
		cgiInf.childExited = false;
		cgiInf.offset = 0;
		cgiInf.startChildTime = std::time(nullptr);
	} catch (std::exception &e) {
		std::cerr << "Failed to init pipes: " << e.what() << std::endl;
		cgiInf.state = ERROR;
		CGIerrorReply(i);
	}
}


void Socket::checkOnChild(int i){

	CGIInfo &cgiInf = _clients[i].cgiInfo;

	if (cgiInf.state == PIPES_INIT || cgiInf.state == WRITE_READY || cgiInf.state == WRITE_DONE || cgiInf.state == READ_READY){
		std::time_t currentTime = std::time(nullptr);
		double timeDiff = std::difftime(currentTime, cgiInf.startChildTime);
		if (timeDiff > TIMEOUT_CGI){
			cgiInf.state = TIMEOUT;
			kill(cgiInf.childPid, SIGTERM); //kill the child process if it takes too long
			CGIerrorReply(i); 
			return ;
		}
	}
	if (cgiInf.state == WRITE_READY || cgiInf.state == READ_READY){
		try {
			int stChild;
			waitChild(stChild, cgiInf.childPid, cgiInf.childExited);
		} catch (std::exception &e) {
			std::cerr << "Failed child: " << e.what() << std::endl;
			cgiInf.state = ERROR;
			CGIerrorReply(i);
			return ;
		}
	}
	
}

void Socket::pickCGIState(int i){

	CGIInfo &cgiInf = _clients[i].cgiInfo;

	checkOnChild(i); //additional child health checks
	if (cgiInf.state == NO_PIPES){
		return ;
	} else if (cgiInf.state == TIMEOUT || cgiInf.state == ERROR){
		// CGIerrorReply(i);
		return ;
	} else if (cgiInf.state == WRITE_DONE){
		try {
			int stChild;
			waitChild(stChild, cgiInf.childPid, cgiInf.childExited);
			if (cgiInf.childExited == true && stChild != -1){
				cgiInf.state = READ_READY;
			} else if (stChild == -1){
				cgiInf.state = ERROR;
				CGIerrorReply(i);
				return ;
			}
		} catch (std::exception &e) {
			std::cerr << "Failed to wait for child: " << e.what() << std::endl;
			cgiInf.state = ERROR;
			CGIerrorReply(i);
			return ;
		}
	} else {
		if (cgiInf.vCGI.size() == 2){
			struct pollfd &writeFd = cgiInf.vCGI[0]; //expect POLLOUT
			if (cgiInf.state == PIPES_INIT && (writeFd.revents & POLLOUT) == POLLOUT){
				cgiInf.state = WRITE_READY;
			}
		} else if (cgiInf.vCGI.size() == 1) {
			struct pollfd &readFd = cgiInf.vCGI[0]; //expect POLLIN
			if (cgiInf.state == WRITE_DONE && (readFd.revents & POLLIN)== POLLIN){ // && (readFd.revents & POLLIN)== POLLIN)
				cgiInf.state = READ_READY;
			} else if (cgiInf.state == READ_READY && (readFd.revents & POLLHUP)== POLLHUP){
				cgiInf.state = READ_DONE;
				cgiInf.vCGI.erase(cgiInf.vCGI.begin());
				cgiInf.vCGIsize = 0;
			}
		}

	}

}

void	Socket::checkCGIevens(int i){ 

	if (_clients[i].CgiDone == true) //  || _clients[i].cgiInfo.state == ERROR || _clients[i].cgiInfo.state == TIMEOUT
		return ;
	pickCGIState(i);
	if (_clients[i].cgiInfo.state == NO_PIPES){ 
		startChild(i);

	} else if (_clients[i].cgiInfo.state == WRITE_READY){
		writeInChild(i);
		
	} else if (_clients[i].cgiInfo.state == READ_READY){ //read from child
		readFromChild(i);

	} else if (_clients[i].cgiInfo.state == READ_DONE){ 

		parseCorrectResponseCGI(_clients[i].cgiReply, _clients[i].ClientResponse);
		_clients[i].CgiDone = true;
		_vFds[i].events |= POLLOUT; // &= ~POLLOUT;
		_clients[i].reply = _clients[i].ClientResponse.header + _clients[i].ClientResponse.body;
		_clients[i].biteToSend = _clients[i].reply.length();
	}
}


void	Socket::clientTimeout(int i){

	std::time_t currentTime = std::time(nullptr);
	double timeDiff = std::difftime(currentTime, _clients[i].startTime);
	if (timeDiff > TIMEOUT_CLIENT && _clients[i].timeout == false){
		std::cerr << "Client timeout" << std::endl;
		std::cerr << "current timeout limit is : " << TIMEOUT_CLIENT << std::endl;
		try {
			closeClientConnection(i);
		} catch(const std::exception& e) {
			std::cerr << e.what() << std::endl;
		}
	}

	//this approach might interfear with rule poll before read and write, though it is beautifull visually
	// if (_clients[i].timeout == true){
	// 	try {
	// 		//closeClientConnection(i);
	// 		sendData(i);
	// 		return ;
	// 	} catch (std::exception &e) {
	// 		std::cerr << "failed to send data with i = " << i << " and FD: " << _vFds[i].fd << "err message: " << e.what() << std::endl;
	// 	}
				
	// } 		
	// std::time_t currentTime = std::time(nullptr);
	// double timeDiff = std::difftime(currentTime, _clients[i].startTime);
	// if (timeDiff > TIMEOUT_CLIENT && _clients[i].timeout == false){
	// 	std::string hostPort = _hostName + ":" + _portNumber;
	// 	_clients[i].ClientRequest.code = 408;
	// 	try {
	// 		_clients[i].ClientRequest.method = ERR;
	// 		_clients[i].reply = formResponseString(responseStructConstruct(*_servers, hostPort, "", _clients[i].ClientRequest));
	// 		_clients[i].biteToSend = _clients[i].reply.length();
	// 		_vFds[i].events |= POLLOUT; - for send data
	// 		//_vFds[i].events &= ~POLLHUP;
	// 		_clients[i].timeout = true;
	// 	} catch(const std::exception& e) {
	// 		std::cerr << e.what() << std::endl;
	// 	}
	// }
}

void	Socket::checkEvents(){

	for (int i = 0; i < (int)_vFds.size(); i++){
		
		if ((_vFds[i].revents & POLLIN) == POLLIN){
			if (_vFds[i].fd == _listenFd){
				try {
					acceptNewConnect(i); //set time on accept
				} catch (std::exception &e) {
					std::cerr << "failed with i = " << i << " and FD: " << _vFds[i].fd << "err message: " << e.what() << std::endl;
				}
			} else {
				try {
					_clients[i].startTime = std::time(nullptr);//reset unpon receive
					recvConnection(i);
				} catch (std::exception &e) {
					std::cerr << "failed with i = " << i << " and FD: " << _vFds[i].fd << "err message: " << e.what() << std::endl;
				}
			}
		} else if ((_vFds[i].revents & POLLOUT) == POLLOUT){
			try {
				sendData(i);
			} catch (std::exception &e) {
				std::cerr << "failed to send data with i = " << i << " and FD: " << _vFds[i].fd << "err message: " << e.what() << std::endl;
			}
		} else if ((_vFds[i].revents & POLLHUP) == POLLHUP){
				std::cout << "lost connection POLLHUP for fd = " << _vFds[i].fd << std::endl;
				closeClientConnection(i);
		}

		//CGI event check
		if (i > 0 && _clients.size() > i && _clients[i].isCGI == true){
			checkCGIevens(i);
		}
		if (i > 0 && _clients.size() > i){
			clientTimeout(i);
		}
	}
}
