//
//  spawnProcess.cpp
//  server_xcode_project
//
//  Created by Julia Demura on 05/03/2023.
//

#include "spawnProcess.hpp"
#include <string>
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <poll.h>

void freeEnvp(char **envp) {
	//std::cout << "=====free envp here====" << std::endl;
	if (envp == NULL) {
		return;
	}
	for (int i = 0; i < 18; ++i) {
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


char **envpGenerateNew(parsRequest request, std::string portNumberSocket, std::string hostNameSocket){
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
	
	std::string str10 = "PATH_INFO=" + request.hostNameHeader;
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
	envp[18] = NULL;
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

std::vector<pollfd> generatePipes(int* pipeFdIn, int* pipeFdOut){

	// int pipeFdIn[2];
	//int pipeFdOut[2];
	std::vector<pollfd> vPipesCGI;

	if (pipe(pipeFdIn) == -1 || pipe(pipeFdOut) == -1){
		// freeEnvp(envp);
		// std::cerr << "spawnProcess : pipe" << std::endl;
		throw std::runtime_error("spawnProcess : pipe");
	}
	//std::cout << "-----non blocking-----" << std::endl;
	if (!makeNonBlocking(pipeFdIn[0]) || !makeNonBlocking(pipeFdIn[1]) || !makeNonBlocking(pipeFdOut[0]) || !makeNonBlocking(pipeFdOut[1])){
		// freeEnvp(envp);
		// std::cerr << "spawnProcess : fcntl" << std::endl;
		throw std::runtime_error("spawnProcess : fcntl");
	}
	//put pipes in vector for vCGI in Socket
	pollfd pollFdIn;
	pollFdIn.fd = pipeFdIn[0];
	pollFdIn.events = POLLIN;
	pollFdIn.revents = 0;
	vPipesCGI.push_back(pollFdIn);
	pollfd pollFdOut;
	pollFdOut.fd = pipeFdOut[1];
	pollFdOut.events = POLLOUT;
	pollFdOut.revents = 0;
	vPipesCGI.push_back(pollFdOut);
	return vPipesCGI;
}

std::string	spawnProcess(parsRequest request, std::string& portNumSocket, std::string& hostNameSocket, int &statusChild, std::string save_location) {
	
	std::string reply;
	pid_t childPid;
	int pipeFdIn[2];
	int pipeFdOut[2];
	//std::cout << "-----start spawn process-----" << std::endl;
	char *arguments[2];
	arguments[0] = (char *)request.physicalPathCgi.c_str();
	arguments[1] = NULL;
	
	//std::cout << "-----envp generate-----" << std::endl;
	char **envp = envpGenerateNew(request, portNumSocket, hostNameSocket);
	if (envp == NULL){
		std::cerr << "spawnProcess : new" << std::endl;
		throw std::runtime_error("spawnProcess : new");
	}
	if (save_location != "SAVE_LOCATION=") {
		envp[18] = (char *)save_location.c_str();
	}
	// std::cout << "saveloc=" << envp[18] << std::endl;
	// } else {
	// 	std::cout << "no saveloc" << std::endl;
	// }
	// for (int i = 0; envp[i] != NULL; i++){
	// 	if (i == 19)
	// 		break;
	// 	std::cout << envp[i] << std::endl;
	// }
	// std::cout << "-----after envp-----" << std::endl;

	try{
		generatePipes(pipeFdIn, pipeFdOut);
	} catch (std::exception &e){
		std::cerr << e.what() << std::endl;
		freeEnvp(envp);
		throw std::runtime_error("spawnProcess : generatePipes");
	}

	//initianing 2 pipes and making all ends of both pipes non-blocking
	//std::cout << "-----pipe init-----" << std::endl;
	if (pipe(pipeFdIn) == -1 || pipe(pipeFdOut) == -1){
		freeEnvp(envp);
		std::cerr << "spawnProcess : pipe" << std::endl;
		throw std::runtime_error("spawnProcess : pipe");
	}
	//std::cout << "-----non blocking-----" << std::endl;
	if (!makeNonBlocking(pipeFdIn[0]) || !makeNonBlocking(pipeFdIn[1]) || !makeNonBlocking(pipeFdOut[0]) || !makeNonBlocking(pipeFdOut[1])){
		freeEnvp(envp);
		std::cerr << "spawnProcess : fcntl" << std::endl;
		throw std::runtime_error("spawnProcess : fcntl");
	}

	
	//spawn a child process
	childPid = fork();
	//std::cerr << "----fork process, pid: " << childPid << std::endl;
	if (childPid < 0){ //fork failed
		close(pipeFdIn[0]);
		close(pipeFdIn[1]);
		close(pipeFdOut[0]);
		close(pipeFdOut[1]);
		freeEnvp(envp);
		std::cerr << "spawnProcess : fork" << std::endl;
		throw std::runtime_error("spawnProcess : fork");
	}
	else if (childPid == 0){ //in child process
		//std::cerr << "----in child----" << std::endl;
		if (dup2(pipeFdIn[0], STDIN_FILENO) < 0){
			freeEnvp(envp);
			std::cerr << "child dup2 1" << std::endl;
			exit(1);
		}
		if (dup2(pipeFdOut[1], STDOUT_FILENO) < 0){
			freeEnvp(envp);
			std::cerr << "child dup2 2" << std::endl;
			exit(1);
		}
		//std::cerr << "----in child: dup2 2 times done" << std::endl;
		close(pipeFdIn[0]);
		close(pipeFdIn[1]);
		close(pipeFdOut[0]);
		close(pipeFdOut[1]);
		//std::cerr << "----in child: before execve with arg[0] == " << arguments[0] << std::endl;
		//execve(arguments[0], arguments, envp);
		// chdir/some/path/
		//execve executable.cgi
		// std::string newDir;
		// size_t pos = request.physicalPathCgi.rfind('/');
		// if (pos != std::string::npos){
		// 	newDir = request.physicalPathCgi.substr(2, pos - 2);
		// }
		// if (!chdir(newDir.c_str())){
		// 	std::cerr << "could not change directory to " << newDir << std::endl; 
		// 	execve((char *)request.physicalPathCgi.c_str(), NULL, envp);
		// } else {
		// 	std::cerr << "changed directory to " << newDir << std::endl; 
		// 	std::string executable = request.physicalPathCgi.substr(pos + 1);
		// 	execve((char *)executable.c_str(), NULL, envp);
		// }
		execve((char *)request.physicalPathCgi.c_str(), NULL, envp);
		freeEnvp(envp);
		std::cerr << "child execve failed" << std::endl;
		exit(1); //HOW do I end child process in case of execve failure without exit?
		
	} else { //in parent process
		//std::cerr << "----in parent : write in child block----" << std::endl;

		//write in child block
		const char* data = request.requestBody.c_str();
		size_t len = request.requestBody.length();
		ssize_t n = 0;
		//std::cout << "print data that I will write into child: " << request.requestBody << std::endl;
		while (len > 0) {
			if (len > 8192){
				n = write(pipeFdIn[1], data, 8192);
			} else {
				n = write(pipeFdIn[1], data, len);
			}
			//std::cout << "write into child n = " << n << std::endl;
			//std::cout << "print data that I will write into child: " << data << std::endl;
			if (n < 0) {
				std::cerr << "parent : write" << std::endl;
				close(pipeFdIn[0]);
				close(pipeFdIn[1]);
				close(pipeFdOut[0]);
				close(pipeFdOut[1]);
				freeEnvp(envp);
				throw std::runtime_error("spawnProcess: write");
			}
			data += n;
			len -= n;
		}
		close(pipeFdIn[1]);
		close(pipeFdIn[0]);

		
		//waiting for child to proceed
		//std::cout << "----in parent: waiting for child to proceed---- " << std::endl;
		int status;
		if (waitpid(childPid, &status, 0) < 0){
			freeEnvp(envp);
			std::cerr << "spawnProcess: waitpid" << std::endl;
			throw std::runtime_error("spawnProcess: waitpid");
		}
		//std::cout << "----in parent: checking child status--- " << std::endl;
		if (WIFEXITED(status)){
			statusChild = WEXITSTATUS(status);
			if (statusChild == 1){
				statusChild = -1;
				close(pipeFdOut[1]);
				close(pipeFdOut[0]);
				freeEnvp(envp);
				std::cerr << "execve failed" << std::endl;
				throw std::runtime_error("spawnProcess : execve");
			}
		} else {
			statusChild = -1;
			close(pipeFdOut[1]);
			close(pipeFdOut[0]);
			freeEnvp(envp);
			std::cerr << "parent: status child failure" << std::endl;
			throw std::runtime_error("spawnProcess : execve");
		}
		close(pipeFdOut[1]);

		//read from child block
		//std::cout << "----in parent: read from child block---- " << std::endl;
		size_t res = 1;
		char buff[1024];
		memset(buff, 0, 1024);
		while (res > 0){
			res = read(pipeFdOut[0], buff, 1023);
			if (res < 0){
				close(pipeFdOut[0]);
				freeEnvp(envp);
				std::cerr << "parent spawnProcess : read" << std::endl;
				throw std::runtime_error("spawnProcess : read");
			}
			else if (res == 0){
				buff[res] = '\0';
				reply.append(buff, res);
				break;
			} else {
				buff[res] = '\0';
				reply.append(buff, res);
			}
			memset(buff, 0, 1024);
		}
		close(pipeFdOut[0]);
	}
	freeEnvp(envp);
	return reply;
}


