//
//  main.cpp
//  server_xcode_project
//
//  Created by Julia Demura on 09/01/2023.
//

#include <iostream>
#include "Socket.hpp"
#include <fstream>

void	smallTest(const char *port){
	try {
		std::fstream fs;
		fs.open("/Users/yuliia/Codam/webserv/info_serv.strings", std::fstream::in);

		Socket test((void *)port);
		int fd = test.getSocketFd();
		printf("FD: %i\n", fd);
		test.setupSocket();

		//acceptServ(test.getSocketFd());
	} catch (std::exception &e) {
		std::cerr << e.what() << std::endl;
	}
}


int main(int argc, const char * argv[]) {

	if (argc != 2){
		std::cout << "not enough arguments" << std::endl;
	}
	smallTest("8888");

	return 0;
}
