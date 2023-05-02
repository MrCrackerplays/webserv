//
//  spawnProcess.cpp
//  server_xcode_project
//
//  Created by Julia Demura on 05/03/2023.
//

#include "spawnProcess.hpp"

void freeEnvp(char **envp) {
	//std::cout << "=====free envp here====" << std::endl;
	if (envp == NULL) {
		return;
	}
	for (int i = 0; i < 19; ++i) {
		//std::cout << i << std::endl;
		if (envp[i] == NULL)
			i++;
		else
			delete[] envp[i];
	}
	//std::cout << "deleted parts" << std::endl;
	delete[] envp;
	//std::cout << "deleted envp" << std::endl;
}

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


char **envpGenerate(parsRequest request, std::string portNumberSocket, std::string hostNameSocket){
	char **envp = new char*[20];
	if (envp == nullptr)
		return nullptr;
	
	std::string str0 = "SERVER_SOFTWARE=webserv/1.0";
	envp[0] = new char[str0.length() + 1];
	strcpy(envp[0], str0.c_str());
	
	std::string str1 = "GATEWAY_INTERFACE=CGI/1.1";
	envp[1] = new char[str1.length() + 1];
	strcpy(envp[1], str1.c_str());
	
	size_t pos = request.urlPath.find('/');
	if (pos == std::string::npos){
		pos = 0;
	}
	
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
	
	std::string str8 = "CONTENT_LENGTH=" + std::to_string(request.requestBodyLen);
	envp[8] = new char[str8.length() + 1];
	strcpy(envp[8], str8.c_str());
	
	pos = request.urlPath.find('/');
	if (pos == std::string::npos){
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

	// std::string str18 = "SCRIPT_FILENAME=";
	// envp[18] = new char[str18.length() + 1];
	// strcpy(envp[18], str18.c_str());

	// std::string str19 = "SCRIPT_URI=" + request.urlPath;
	// envp[19] = new char[str19.length() + 1];
	// strcpy(envp[19], str19.c_str());
	std::string str18 = request.save_location;
	envp[18] = new char[str18.length() + 1];
	strcpy(envp[18], str18.c_str());
	envp[19] = NULL;

	return envp;
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


size_t	writeChild(const char* data, size_t dataLen, int* pipeFdIn){ //have to do it in multiple calls

	ssize_t n = 0;

	if (dataLen > 0) {
		if (dataLen > 8192){
			n = write(pipeFdIn[1], data, 8192);
		} else {
			n = write(pipeFdIn[1], data, dataLen);
		}
		if (n < 0) {
			throw std::runtime_error("SpawnProcess: writeInChild : write");
		}
	}
	if (dataLen == 0){
		close(pipeFdIn[1]);
		return 0;
	}
	return n;
}


size_t	readChild(int* pipeFdOut, std::string &reply){ //have to do it in multiple calls

	size_t res = 1;
	char buff[1024];
	memset(buff, 0, 1024);

	res = read(pipeFdOut[0], buff, 1023);
	if (res < 0){
		//handle error
		throw std::runtime_error("SpawnProcess: readFromChild : read");
	}
	buff[res] = '\0';
	//std::cout << "************buff from child: " << buff << std::endl;
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
	//std::cout << "---- wait for child ----" << std::endl;
	int status;
	pid_t wpidRes = waitpid(childPid, &status, WNOHANG);
	if (wpidRes == 0){
		childExited = false;
		//std::cout << "child is still running" << std::endl;
	} else if (wpidRes < 0){ 
		if (wpidRes != -1){ //if this is -1 continue WNOHANG
			std::cerr << "spawnProcess: waitpid" << std::endl;
			throw std::runtime_error("spawnProcess: waitpid");
		}
	} else {
		childExited = true;
		//std::cout << "---- child is done (from wait) ----" << std::endl;
		if (WIFEXITED(status)){
			statusChild = WEXITSTATUS(status);
			//std::cout << "child exited with status: " << statusChild << std::endl;
			if (statusChild == 1){
				statusChild = -1;
				// std::cerr << "cgi failed" << std::endl;
				// throw std::runtime_error("spawnProcess : execve");
			}
		} else {
			statusChild = -1;
			// std::cerr << "parent: status child failure" << std::endl;
			// throw std::runtime_error("spawnProcess : execve");
		}
	}
	
}

pid_t	launchChild(CGIInfo &info, parsRequest &request, std::string& portNumSocket, std::string& hostNameSocket){

	//std::cout << request.physicalPathCgi << std::endl;
	try{
		initPipesCreatePollFDstruct(info.vCGI, info.pipeFdIn, info.pipeFdOut);
		info.vCGIsize = 2;
	} catch (std::exception &e){
		std::cerr << e.what() << std::endl;
		throw std::runtime_error("spawnProcess : generatePipes");
	}

	info.envp = envpGenerate(request, portNumSocket, hostNameSocket);
	if (info.envp == NULL){
		closePipes(info.pipeFdIn, info.pipeFdOut);
		std::cerr << "spawnProcess : new" << std::endl;
		throw std::runtime_error("spawnProcess : new");
	}

	info.childPid = fork();
	if (info.childPid < 0){ //fork failed
		closePipes(info.pipeFdIn, info.pipeFdOut);
		//freeEnvp(info.envp);
		std::cerr << "spawnProcess : fork" << std::endl;
		throw std::runtime_error("spawnProcess : fork");
	}
	if (info.childPid == 0){		//in child process
			std::cout << "--- in child ---- " << std::endl;
		if (dup2(info.pipeFdIn[0], STDIN_FILENO) < 0){
			//freeEnvp(info.envp);
			closePipes(info.pipeFdIn, info.pipeFdOut);
			std::cerr << "child dup2 1" << std::endl;
			exit(1);
		}
		if (dup2(info.pipeFdOut[1], STDOUT_FILENO) < 0){
			//freeEnvp(info.envp);
			closePipes(info.pipeFdIn, info.pipeFdOut);
			std::cerr << "child dup2 2" << std::endl;
			exit(1);
		}

	

		// closePipes(info.pipeFdIn, info.pipeFdOut);

		// std::cout << "---- before execve:----" << std::endl;
		// std::cerr << "pipeFdIn[0] = " << info.pipeFdIn[0] << std::endl;
		// std::cerr << "pipeFdIn[1] = " << info.pipeFdIn[1] << std::endl;
		// std::cerr << "pipeFdOut[0] = " << info.pipeFdOut[0] << std::endl;
		// std::cerr << "pipeFdOut[1] = " << info.pipeFdOut[1] << std::endl;
		// std::cout << "------------------------------" << std::endl;
		close(info.pipeFdIn[0]);
		close(info.pipeFdIn[1]);
		close(info.pipeFdOut[0]);
		close(info.pipeFdOut[1]);
		//std::cerr << "child execve now :" << std::endl;
		//std::cout << "before execve: path : " << request.physicalPathCgi << std::endl;
		execve((char *)request.physicalPathCgi.c_str(), NULL, info.envp);
		//freeEnvp(info.envp);
		std::cerr << "child execve failed" << std::endl;
		exit(1);
	} else {
		//std::cout << "--- in parent from launchChild---- " << std::endl;
		//in parent process
		
		close(info.pipeFdIn[0]);
		//info.pipeFdIn[0] = -1;
		close(info.pipeFdOut[1]); 
		//info.pipeFdOut[1] = -1;
		
		// int statusChild;
		// waitChild(statusChild, info.childPid);
		//std::cout << "end of launchChild ----- child pid: " << info.childPid << std::endl;
		//pollhup will notify tat clild is done
	}
	
	return info.childPid;
}














// void	inChildProcess(int* pipeFdIn, int* pipeFdOut, char **envp, char *path){

// 	if (dup2(pipeFdIn[0], STDIN_FILENO) < 0){
// 			freeEnvp(envp);
// 			std::cerr << "child dup2 1" << std::endl;
// 			exit(1);
// 		}
// 		if (dup2(pipeFdOut[1], STDOUT_FILENO) < 0){
// 			freeEnvp(envp);
// 			std::cerr << "child dup2 2" << std::endl;
// 			exit(1);
// 		}
// 		closePipes(pipeFdIn, pipeFdOut);
// 		execve(path, NULL, envp);
// 		freeEnvp(envp);
// 		std::cerr << "child execve failed" << std::endl;
// 		exit(1);
// }
