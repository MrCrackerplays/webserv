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
	//std::cout << "getCGIVector for i= "<< i << std::endl;
	// if (_clients.size() < i)
	// 	return _clients[0].cgiInfo.vCGI;
	return _clients[i].cgiInfo.vCGI;
};

size_t	Socket::getCGIVectorSize(int i){
	return _clients[i].cgiInfo.vCGI.size();
};

bool	Socket::getCGIbool(int i){
	if (_vFdsSize < i){
		return false;
	}
	if (!_clients[i].isCGI){
		return false;
	}
	return _clients[i].isCGI;
};

void Socket::setPollFdVectorSize(size_t size) { _vFdsSize = size; }

size_t Socket::numberOfConnections(){

	size_t totalCguFdSize = 0;
	size_t clientIndex = 0;
	size_t clientvectSize = _clients.size();
	for (std::vector<struct pollfd>::iterator it = _vFds.begin(); it != _vFds.end(); it++){
		if (clientIndex > clientvectSize){
			std::cout << "clientIndex > clientvectSize" << std::endl;
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
	
	// std::cout << "=====unpackVectorintoSocket======" << std::endl;
	// std::cout << "allCounter = " << allCounter << std::endl;
	// std::cout << "fdCounter = " << fdCounter << std::endl;
	// std::cout << "socketsAll.size() = " << socketsAll.size() << std::endl;
	// std::cout << "_vFds.size() = " << _vFds.size() << std::endl;
	

	if (_vFds.size() < fdCounter){
		std::cout << "_vFds.size() < fdCounter but this should not happen" << std::endl;
		return;
	}
	_vFds[fdCounter].fd = socketsAll[allCounter].fd;
	_vFds[fdCounter].events = socketsAll[allCounter].events;
	_vFds[fdCounter].revents = socketsAll[allCounter].revents;
	allCounter++;

	if (_clients.size() > fdCounter && _clients[fdCounter].isCGI){
		CGIInfo &CGIinfo = _clients[fdCounter].cgiInfo;
			//std::cout << "_clients[fdConter].cgiInfo.vCGISize = " << _clients[fdCounter].cgiInfo.vCGIsize<< std::endl;	
			//std::cout << "FD is = "<< socketsAll[allCounter].fd << std::endl;
			if (CGIinfo.vCGI.size() == 1) {
				if (socketsAll.size() < allCounter + 1){
					std::cout << "socketsAll.size() < allCounter" << std::endl;
					return;
				}
			//	std::cout << "CGIinfo.vCGI.size() == 1 ONE" << std::endl;
				CGIinfo.vCGI[0].fd = socketsAll[allCounter].fd;
				CGIinfo.vCGI[0].events = socketsAll[allCounter].events;
				CGIinfo.vCGI[0].revents = socketsAll[allCounter].revents;
				allCounter++;
			//	std::cout << "CGIinfo.vCGI[0].fd = " << CGIinfo.vCGI[0].fd << std::endl;
			} else if (CGIinfo.vCGI.size() == 2) {
				if (socketsAll.size() < allCounter + 2){
			//		std::cout << "socketsAll.size() < allCounter" << std::endl;
					return;
				}
			//	std::cout << "CGIinfo.vCGI.size() == 2 TWO" << std::endl;
				CGIinfo.vCGI[0].fd = socketsAll[allCounter].fd;
				CGIinfo.vCGI[0].events = socketsAll[allCounter].events;
				CGIinfo.vCGI[0].revents = socketsAll[allCounter].revents;
				allCounter++;
				CGIinfo.vCGI[1].fd = socketsAll[allCounter].fd;
				CGIinfo.vCGI[1].events = socketsAll[allCounter].events;
				CGIinfo.vCGI[1].revents = socketsAll[allCounter].revents;
				allCounter++;
			//	std::cout << "CGIinfo.vCGI[1].fd = " << CGIinfo.vCGI[1].fd << std::endl;
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
	
	// _addrinfo = new struct addrinfo;
	_addrinfo = new struct addrinfo;
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
	//UNFINISHED
	//std::cout << "Socket destructor DESTRUCTIONNNNN" << std::endl;
	if (_addrinfo) {
		freeaddrinfo(_addrinfo); //pointer being freed after allocating with new - check needed
		delete _addrinfo;
	}
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
		struct pollfd newPollfd;
		newPollfd.fd = newFd;
		newPollfd.events = POLLIN | POLLHUP; //added POLLHUP though it may be not proper place to add both
		_vFds.push_back(newPollfd);
		_vFdsSize = _vFds.size();
		std::cout << "new connection accepted, _vFds.size = " << _vFdsSize << std::endl;
		
		//initiate client.struct
		ClientInfo clientStruct;
		clientStruct.biteToSend = 0;
		clientStruct.biteSentInt = 0;
		clientStruct.recvBytes = 0;
		clientStruct.receivedContent.clear();
		clientStruct.receivedContentVector.clear();
		clientStruct.reply.clear();
		clientStruct.isCGI = false;
		_clients.push_back(clientStruct); //erase it when time comes UNFINISHED
	}
}

void transferBufferToVector(const char* buff, ssize_t buffSize, std::vector<char>& rowData) {
    rowData.insert(rowData.end(), buff, buff + buffSize);
}

void removeHeaderFromRowData(std::vector<char>& rowData) {
	std::cout << "removeHeaderFromRowData" << std::endl;
	std::cout << "receivedContentVector.size() = " << rowData.size() << std::endl;
    const std::string doubleCRLF = "\r\n\r\n";

    std::string rowDataStr(rowData.begin(), rowData.end());
    size_t headerEndPos = rowDataStr.find(doubleCRLF);

    if (headerEndPos != std::string::npos) {
		std::cout << "headerEnd != rowData.end()***************************************************" << std::endl;
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
		std::cout << "res == -1 we go on" << std::endl;
		return;
	}
	else if (res < 0){
		closeClientConnection(i);
		throw std::runtime_error("SockedLoop : recv");
	} else if (res > 0) {
		transferBufferToVector(buff, res, _clients[i].receivedContentVector);
		std::cout << "receivedContentVector = " << _clients[i].receivedContentVector.size() << std::endl;
		//std::cout << _clients[i].receivedContentVector.size() << std::endl;
		//old approach used to check if request is full
		buff[res] = '\0';
		_clients[i].recvBytes += res;
		_clients[i].receivedContent.append(buff, res);
		if (fullRequestReceived(_clients[i].receivedContent, _clients[i].recvBytes, res)){
			//parsing part
			try {
				_clients[i].ClientRequest.parsBuff = _clients[i].receivedContent;
				std::cout << "before methods , receivedContentVector = " << _clients[i].receivedContentVector.size() << std::endl;
				_clients[i].reply = methods(_clients[i].ClientRequest, *_servers, _portNumber, _hostName, _clients[i].isCGI);
				std::cout << "after methods, receivedContentVector = " << _clients[i].receivedContentVector.size() << std::endl;
				if (_clients[i].isCGI == false){
					_vFds[i].events |= POLLOUT;
					_clients[i].biteToSend = _clients[i].reply.size();
				}
				else {
					std::cout << "vector before header removal , receivedContentVector = " << _clients[i].receivedContentVector.size() << std::endl;
					removeHeaderFromRowData(_clients[i].receivedContentVector);
					std::cout << "vector after header removal , receivedContentVector = " << _clients[i].receivedContentVector.size() << std::endl;
					_clients[i].cgiInfo.state = NO_PIPES;
					_clients[i].CgiDone = false;
					_clients[i].cgiInfo.childPid = 0;
				}
			} catch (std::exception &e) {
				std::cerr << "Caught exception: " << e.what() << std::endl;
			}
		}
	}
}

void	Socket::closeClientConnection(int i){
	std::cout << "closeClientConnection for fd = " << _vFds[i].fd << std::endl;
	close(_vFds[i].fd);
	_vFds.erase(_vFds.begin() + i, _vFds.begin() + i + 1);
	_vFdsSize--;
	_clients[i].receivedContent.clear();
	_clients[i].receivedContentVector.clear();
	_clients.erase(_clients.begin() + i, _clients.begin() + i + 1);
}

void	Socket::sendData(int i){

	// if (i > 0 && _clients.size() > i && _clients[i].isCGI == true){	
	// 	std::cout << "biteToSend: " << _clients[i].biteToSend << std::endl;
	// 	std::cout << "reply: " << _clients[i].reply << std::endl;
	// }

	ssize_t biteSent = send(_vFds[i].fd, _clients[i].reply.c_str(), _clients[i].reply.length(), 0);
	if (i > 0 && _clients.size() > i && _clients[i].isCGI == true){
		// std::cout << "biteSent for cgi reply: " << biteSent << std::endl;
		// std::cout << "reply: " << _clients[i].reply << std::endl;
		// std::cout << "send to fd =" << _vFds[i].fd << std::endl; // ->> check if I do not close it somewhere previously
	}
	if (biteSent == -1){
		std::cerr << "biteSent == -1" << std::endl;
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
		readChild(_clients[i].cgiInfo.pipeFdOut, _clients[i].cgiReply);
		//std::cout << "reply from child: " << _clients[i].reply << std::endl;
		//std::cout << "---- read from child end----" << std::endl;
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
		//ssize_t wrote = writeChild(client.ClientRequest.requestBody.c_str(), client.ClientRequest.requestBody.length(), cgiInf.pipeFdIn);
		// if (wrote == -1){
		// 	return ;
		// }

		//client.ClientRequest.requestBody.erase(0, wrote);
		std::vector<char> &rowData = client.receivedContentVector;
		ssize_t wrote = writeChild(rowData, client.cgiInfo.offset, cgiInf.pipeFdIn);
		if (wrote == -1) {
    		return;
		}

		rowData.erase(rowData.begin(), rowData.begin() + wrote);



		if (client.ClientRequest.requestBody.empty() || wrote == 0){
			std::cout << "write done" << std::endl;
			cgiInf.state = WRITE_DONE;
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
	if (cgiInf.state == NO_PIPES){
		std::cout << "no pipes yet" << std::endl;
		return ;
	} else if (cgiInf.state == WRITE_DONE){
		try {
			int stChild;
			waitChild(stChild, cgiInf.childPid, cgiInf.childExited);
			if (cgiInf.childExited == true){ // && stChild > 0
				cgiInf.state = READ_READY;
				std::cout << "read ready, vCGi size is " << cgiInf.vCGI.size() <<  std::endl;
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
				std::cout << "write ready, vCGi size is " << cgiInf.vCGI.size() <<  std::endl;
			}
		} else if (cgiInf.vCGI.size() == 1) {
			struct pollfd &readFd = cgiInf.vCGI[0]; //expect POLLIN
			if (cgiInf.state == WRITE_DONE && (readFd.revents & POLLIN)== POLLIN){ // && (readFd.revents & POLLIN)== POLLIN)
				cgiInf.state = READ_READY;
				std::cout << "READ_READY, vCGi size is " << cgiInf.vCGI.size() <<  std::endl;
			} else if (cgiInf.state == READ_READY && (readFd.revents & POLLHUP)== POLLHUP){
				cgiInf.state = READ_DONE;
				cgiInf.vCGI.erase(cgiInf.vCGI.begin()); //read in vCGI[1] but write should be erased already NOT SURE
				cgiInf.vCGIsize = 0;
				std::cout << "READ_DONE, vCGi size is expected 0 and is: " << cgiInf.vCGI.size() <<  std::endl;
			}
		}

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

	} else if (_clients[i].cgiInfo.state == READ_DONE){ 

		parseCorrectResponseCGI(_clients[i].cgiReply, _clients[i].ClientResponse);
		_clients[i].CgiDone = true;
		_vFds[i].events |= POLLOUT; // &= ~POLLOUT;
		_clients[i].reply = _clients[i].ClientResponse.header + _clients[i].ClientResponse.body;
		_clients[i].biteToSend = _clients[i].reply.length();
	}
}

void	Socket::checkEvents(){
	// std::cout << "====checkEvents===" << std::endl;
	
	// std::cout << "vFds size = " << _vFds.size() << std::endl;
	// std::cout << "_listenFd = " << _listenFd << std::endl;
	// int i = 0;
	// for (std::vector<struct pollfd>::iterator it = _vFds.begin(); it != _vFds.end(); it++){
	// 	std::cout << "fd[" << i << "] = " << it->fd << std::endl;
	// 	i++;
	// }
	// std::cout << "==================" << std::endl;
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
			//std::cout << "after cgi POLLOUT added to fd " << _vFds[i].fd << " and i = " << i << std::endl;
			try {
				sendData(i);
			} catch (std::exception &e) {
				std::cerr << "failed to send data with i = " << i << " and FD: " << _vFds[i].fd << "err message: " << e.what() << std::endl;
			}
		} else if ((_vFds[i].revents & POLLHUP) == POLLHUP){
				std::cout << "lost connection POLLHUP for fd = " << _vFds[i].fd << std::endl;
				closeClientConnection(i);
		}
		if (i > 0 && _clients.size() > i && _clients[i].isCGI == true){
			
			checkCGIevens(i);
		}
	}
	
}
