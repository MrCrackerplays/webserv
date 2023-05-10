//
//  spawnProcess.cpp
//  server_xcode_project
//
//  Created by Julia Demura on 05/03/2023.
//


#include "spawnProcess.hpp"

void	closePipes(int* pipeFdIn, int* pipeFdOut){
	// if (pipeFdIn == nullptr || pipeFdOut == nullptr)
	// 	return;
	if (pipeFdIn[0] != -1)
		close(pipeFdIn[0]);
	if (pipeFdIn[1] != -1)
		close(pipeFdIn[1]);
	if (pipeFdOut[0] != -1)
		close(pipeFdOut[0]);
	if (pipeFdOut[1] != -1)
		close(pipeFdOut[1]);
}


void	envpGenerate(char** envp, parsRequest request, std::string portNumberSocket, std::string hostNameSocket, size_t contentLenghtCGI) {
	std::string str0 = "SERVER_SOFTWARE=webserv/1.0";
	envp[0] = new char[str0.length() + 1];
	strcpy(envp[0], str0.c_str());
	
	std::string str1 = "GATEWAY_INTERFACE=CGI/1.1";
	envp[1] = new char[str1.length() + 1];
	strcpy(envp[1], str1.c_str());
	
	std::string str2 = "SERVER_NAME=" + hostNameSocket;
	envp[2] = new char[str2.length() + 1];
	strcpy(envp[2], str2.c_str());
	
	std::string str3 = "SERVER_PROTOCOL=" + request.httpVers;
	envp[3] = new char[str3.length() + 1];
	strcpy(envp[3], str3.c_str());
	
	std::string str4 = "SERVER_PORT=" + portNumberSocket;
	envp[4] = new char[str4.length() + 1];
	strcpy(envp[4], str4.c_str());
	
	std::string str5 = "REQUEST_METHOD=" + request.methodString;
	envp[5] = new char[str5.length() + 1];
	strcpy(envp[5], str5.c_str());
	
	std::string str6 = "QUERY_STRING=" + request.queryString;
	envp[6] = new char[str6.length() + 1];
	strcpy(envp[6], str6.c_str());
	
	std::string contType = getHeaderByKey(request.headers, "Content-Type");
	std::string str7 = "CONTENT_TYPE=" + contType;
	envp[7] = new char[str7.length() + 1];
	strcpy(envp[7], str7.c_str());
	

	std::string str8 = "CONTENT_LENGTH=" + std::to_string(contentLenghtCGI);
	envp[8] = new char[str8.length() + 1];
	strcpy(envp[8], str8.c_str());
	
	size_t	pos = request.urlPath.find('/');
	if (pos == std::string::npos) {
		pos = 0;
	}

	std::string scriptN = request.urlPath.substr(pos+1, request.urlPath.length());
	std::string str9 = "SCRIPT_NAME=" + scriptN;
	envp[9] = new char[str9.length() + 1];
	strcpy(envp[9], str9.c_str());
	
	std::string str10 = "PATH_INFO=";
	envp[10] = new char[str10.length() + 1];
	strcpy(envp[10], str10.c_str());
	
	std::string str11 = "PATH_TRANSLATED=" + request.hostNameHeader;
	envp[11] = new char[str11.length() + 1];
	
	std::string str12 = "REMOTE_HOST=";
	envp[12] = new char[str12.length() + 1];
	strcpy(envp[12], str12.c_str());
	
	std::string str13 = "REMOTE_ADDR=";
	envp[13] = new char[str13.length() + 1];
	strcpy(envp[13], str13.c_str());

	std::string str14 = "AUTH_TYPE=";
	envp[14] = new char[str14.length() + 1];
	strcpy(envp[14], str14.c_str());

	std::string str15 = "REMOTE_USER=";
	envp[15] = new char[str15.length() + 1];
	strcpy(envp[15], str15.c_str());

	std::string str16 = "REMOTE_IDENT=";
	envp[16] = new char[str16.length() + 1];
	strcpy(envp[16], str16.c_str());

	std::string str17 = std::string("PATH=") + getenv("PATH");
	envp[17] = new char[str17.length() + 1];
	strcpy(envp[17], str17.c_str());

	std::string str18 = request.save_location;
	envp[18] = new char[str18.length() + 1];
	strcpy(envp[18], str18.c_str());
	
	envp[19] = NULL;
}

bool	makeNonBlocking(int fd){
	int flagsForFd = fcntl(fd, F_GETFL, 0);
	if (flagsForFd < 0){
		return false;
	}
	if (fcntl(fd, F_SETFL, flagsForFd | O_NONBLOCK) < 0){
		return false;
	}
	return true;
}

void	initPipesCreatePollFDstruct(std::vector<struct pollfd> &vPipesCGI, int* pipeFdIn, int* pipeFdOut){ //socket pick

	//set pipes to -1 by default at the start of the function
	pipeFdIn[0] = -1;
	pipeFdIn[1] = -1;
	pipeFdOut[0] = -1;
	pipeFdOut[1] = -1;
	if (pipe(pipeFdIn) == -1 || pipe(pipeFdOut) == -1){
		throw std::runtime_error("spawnProcess : pipe");
	}
	if (!makeNonBlocking(pipeFdIn[0]) || !makeNonBlocking(pipeFdIn[1]) || !makeNonBlocking(pipeFdOut[0]) || !makeNonBlocking(pipeFdOut[1])){
		throw std::runtime_error("spawnProcess : fcntl");
	}
	//put pipes in vector for vCGI in Socket
	//write into pipe child
	struct pollfd pollWrite;
	pollWrite.fd = pipeFdIn[1];
	pollWrite.events = POLLOUT | POLLHUP;
	pollWrite.revents = 0;
	vPipesCGI.push_back(pollWrite);
	//read from pipe clild
	struct pollfd pollRead; 
	pollRead.fd = pipeFdOut[0];
	pollRead.events = POLLIN | POLLHUP;
	pollRead.revents = 0;
	vPipesCGI.push_back(pollRead);
}

ssize_t writeChild(const std::vector<char>& rowData, size_t& offset, int* pipeFdIn) {
    
	ssize_t n = 0;
    size_t remainingDataLen = rowData.size() - offset;
    size_t chunkSize = (remainingDataLen > 8192) ? 8192 : remainingDataLen;
    if (chunkSize > 0) {
        n = write(pipeFdIn[1], rowData.data() + offset, chunkSize);
        if (n < 0 && n != -1) {
            throw std::runtime_error("SpawnProcess: writeInChild: write");
        }
	}
	if (n >= 0){
		offset += n;
   		if (offset == rowData.size()) {
        	close(pipeFdIn[1]);
    	}
	}
    return n;
}


ssize_t	readChild(int* pipeFdOut, std::string &reply){

	ssize_t res = 1;
	char buff[1024];
	memset(buff, 0, 1024);

	res = read(pipeFdOut[0], buff, 1023);
	if (res == -1){
		//DO WE NEED error
		throw std::runtime_error("SpawnProcess: readFromChild : read");
	}
	buff[res] = '\0';
	if (res == 0){
		reply.append(buff, res);
		close(pipeFdOut[0]);
		return 0;
	} else {
		reply.append(buff, res);
		return res;
	}
	return 0;
}

void	waitChild(int &statusChild, pid_t childPid, bool &childExited){
	int status;
	
	// std::cout << "childPid" <<childPid << std::endl;
	pid_t wpidRes = waitpid(childPid, &status, WNOHANG);
	
	
	// std::cout << "wpidRes = " << wpidRes << std::endl;
	// std::cout << "status = " << status << std::endl;
	// std::cout << "WIFEXITED(status) = " << WIFEXITED(status) << std::endl;

	if (wpidRes == 0){
		childExited = false;
	} else if (wpidRes < 0){ 
		if (wpidRes != -1){ //if this is -1 continue WNOHANG
			std::cerr << "spawnProcess: waitpid" << std::endl;
			throw std::runtime_error("spawnProcess: waitpid");
		}
	} else {
		childExited = true;
		if (WIFEXITED(status)){
			statusChild = WEXITSTATUS(status);
			if (statusChild != 0){
				statusChild = -1;
				std::cerr << "cgi failed" << std::endl;
				throw std::runtime_error("spawnProcess : execve");
			}
		} else {
			statusChild = -1;
			std::cerr << "parent: status child failure" << std::endl;
			throw std::runtime_error("spawnProcess : execve");
		}
	}
	
}

// void waitChild(int& statusChild, pid_t childPid, bool& childExited) {
//     std::cout << "----waitChild------" << std::endl;
//     int status;

//     pid_t wpidRes = waitpid(childPid, &status, WNOHANG);

//     std::cout << "wpidRes = " << wpidRes << std::endl;
//     std::cout << "status = " << status << std::endl;
//     std::cout << "WIFEXITED(status) = " << WIFEXITED(status) << std::endl;
//     std::cout << "WIFSTOPPED(status) = " << WIFSTOPPED(status) << std::endl;

//     if (wpidRes == 0) {
//         // Child process is still running
// 		int stoppedStatus;
// 		int stopPid = waitpid(childPid, &stoppedStatus, WNOHANG | WUNTRACED);
       
//         if (stopPid > 0) {
//             if (WIFSTOPPED(stoppedStatus)) {
//                 std::cout << "Child process is stopped." << std::endl;
//                 // Handle the case when child process is stopped (e.g., by a signal)
//                 // Update the childExited flag or take appropriate action
//             }
//         } else if (stopPid != -1){
//             // Error occurred while checking child process status
//             std::cerr << "Error in waitpid: " << strerror(errno) << std::endl;
//             throw std::runtime_error("spawnProcess: waitpid");
//         }
//     } else if (wpidRes < 0) {
//         if (wpidRes != -1) {
//             std::cerr << "spawnProcess: waitpid" << std::endl;
//             throw std::runtime_error("spawnProcess: waitpid");
//         }
//     } else {
//         childExited = true;
//         if (WIFEXITED(status)) {
//             statusChild = WEXITSTATUS(status);
//             if (statusChild != 0) {
//                 statusChild = -1;
//                 std::cerr << "cgi failed" << std::endl;
//                 throw std::runtime_error("spawnProcess : execve");
//             }
//         } else {
//             statusChild = -1;
//             std::cerr << "parent: status child failure" << std::endl;
//             throw std::runtime_error("spawnProcess : execve");
//         }
//     }
// }


pid_t	launchChild(CGIInfo &info, parsRequest &request, std::string& portNumSocket, std::string& hostNameSocket){

	try{
		initPipesCreatePollFDstruct(info.vCGI, info.pipeFdIn, info.pipeFdOut);
		info.vCGIsize = 2;
	} catch (std::exception &e){
		std::cerr << e.what() << std::endl;
		throw std::runtime_error("spawnProcess : generatePipes");
	}

	info.childPid = fork();
	if (info.childPid < 0){ //fork failed
		std::cerr << "spawnProcess : fork" << std::endl;
		closePipes(info.pipeFdIn, info.pipeFdOut);
		throw std::runtime_error("spawnProcess : fork");
	}
	if (info.childPid == 0){		//in child process
		if (dup2(info.pipeFdIn[0], STDIN_FILENO) < 0){
			std::cerr << "child dup2 1" << std::endl;
			closePipes(info.pipeFdIn, info.pipeFdOut);
			exit(1);
		}
		if (dup2(info.pipeFdOut[1], STDOUT_FILENO) < 0){
			std::cerr << "child dup2 2" << std::endl;
			closePipes(info.pipeFdIn, info.pipeFdOut);
			exit(1);
		}
		close(info.pipeFdIn[0]);
		close(info.pipeFdIn[1]);
		close(info.pipeFdOut[0]);
		close(info.pipeFdOut[1]);

		char *envp[20];
		//TEST ISSUE IN CGI
		envpGenerate(envp, request, portNumSocket, hostNameSocket, info.contentLenghtCGI);
		//char ** envp = NULL;
		execve((char *)request.physicalPathCgi.c_str(), NULL, envp);
		std::cerr << "child execve failed" << std::endl;
		exit(1);
	} else {
		close(info.pipeFdIn[0]);
		close(info.pipeFdOut[1]); 
	}
	return info.childPid;
}