//
//  main.cpp
//  server_xcode_project
//
//  Created by Julia Demura on 09/01/2023.
//

#include <iostream>
#include "Socket.hpp"
#include <fstream>

//void	separate_for_test(char *port_number){
//
//	//create sockaddr_in
//	struct sockaddr_in addrStruct;
//	addrStruct.sin_family = AF_INET;
//	int num = atoi(port_number);
//	addrStruct.sin_port = htons(num);
//	addrStruct.sin_addr.s_addr = INADDR_ANY;
//
//
//	//create socket
//	int socketFd = socket(PF_INET, SOCK_STREAM, 0);
//	if (socketFd < 0){
//		throw std::runtime_error("Socket : socket");
//	}
//
//	// bind
//	if (bind(socketFd, (sockaddr*)&addrStruct, sizeof(addrStruct)) < 0){
//		close(socketFd);
//		throw std::runtime_error("Socket : bind");
//	} else {
//		std::cout << "bind sucsessful"  << std::endl;
//	}
//
//	//listen
//	if (listen(socketFd, SOMAXCONN) < 0){
//		close(socketFd);
//		throw std::runtime_error("Socket : bind");
//	}
//	//set to non blocking
//	if (fcntl(socketFd, F_SETFL, O_NONBLOCK) < 0){
//		throw std::runtime_error("Socket : fcntl");
//	}
//
//	//poll
//	struct pollfd *fds;
//	fds = (pollfd *)malloc(sizeof(pollfd) * 5);
//
//	fds->fd = socketFd;
//	fds->events = POLLIN;
//
//	int res = poll(fds, 1, 0);
//	perror("CHECK poll");
//	if (res < 0){
//		throw std::runtime_error("Socket : poll");
//	} else if (res == 0){
//		//Returns 0 if no descriptors are ready before the timer expires
//		throw std::runtime_error("Socket : poll no FD");
//	} else {
//
//		if (fds[0].revents & POLLIN){
//			///one or more file descriptors have events available.
//			///The code checks if the revents field of the fds struct contains the POLLIN flag,
//			///which indicates that the file descriptor has incoming data.
//
//		}
//	}
//}

//**********************************************************

//void	Socket::sendData(int client_socket){
//
//	char server_message[256] = "Hello, Client!";
//	if (send(client_socket, server_message, sizeof(server_message), 0) < 0){
//		throw std::runtime_error("Socket : send");
//	}
//}

//int	incomingConnection(int listenFd){
//	int client_socket = accept(listenFd, NULL, NULL);
//	std::cout << "accepted: " << client_socket << std::endl;
//	if (client_socket < 0){
//		perror("Socket : accept");
//		throw std::runtime_error(std::string("Socket : accept") + strerror(errno));
//	}
//	return client_socket;
//}
//
//void	handleEvents(Socket *Skt){
//
//	for (int i = 0; i < EVENTS_NUM; i++) {
//		if (Skt->getVFd()[i].revents & POLLIN){
//
//			char buff[1024];
//			std::cout<< "handleEvents fd i" << i <<" now: "<< Skt->getVFd()[i].fd << std::endl;
//			int recvRes = (int)recv(Skt->getVFd()[i].fd, buff, sizeof(buff), 0);
//			std::cout << Skt->getVFd()[i].fd << std::endl;
//			perror("recv");
//			if (recvRes < 0) {
//				throw std::runtime_error("Socket : recv"); //not sure
//			} else if (recvRes == 0) {
//				std::cout << "Connection closed by client" << std::endl;
//			} else {
//				//RESPONSE
//				//sendData(_vFds[i].fd);
//			}
//		}
//	}
//}

//void	addInEventQueu(Socket *Skt, int client_socket){
//
//	int allTaken = true;
//	for (int i = 0; i < EVENTS_NUM; i++) {
//		if (Skt->getVFd()[i].fd == 0){
//			Skt->getVFd()[i].fd = client_socket;
//			Skt->getVFd()[i].events = POLLIN;
//			allTaken = false;
//			break;
//		}
//	}
//	if (allTaken == true){
//		//not sure if I need to clean prev sockets or how to proceed
//	}
//}

//void pollLoop(const char *port){
//
//	Socket Skt((void *)port);
//	int incomingFd = 0;
//
//	Skt.initiateVect();
//	while (true) {
//
//		if (poll(Skt.getVFd(), EVENTS_NUM, 0) < 0){
//			throw std::runtime_error("Socket : poll");
//		} else {
//			//std::cout << Skt.getVFd()[0].revents <<std::endl;
//			if (Skt.getVFd()[0].revents & POLLIN){
//				incomingFd = incomingConnection(Skt.getVFd()[0].fd);
//				//addInEventQueu(&Skt, incomingFd);
//
//			}
//			handleEvents(&Skt);
//
//		}
//	}//end of while loop
//
//}

//********************************************



//#include <iostream>
//#include <stdlib.h> // For atoi()
//#include <sys/types.h> // For socket()
//#include <sys/socket.h> // For socket()
//#include <string.h> // For memset()
//#include <unistd.h> // For close()
//#include <arpa/inet.h> // for htons()
//#include <netdb.h>  // Definitions for default buffer sizes.
//#include <sys/select.h> // For select()
//#include <sys/time.h> // for select
//#include <fcntl.h>
//#include <vector>
//#include <poll.h> // for poll()
//
//int         server_port;
//std::string server_password;
//
//void    error_exit(const std::string& msg, int exit_code)
//{
//	std::cerr << msg << std::endl;
//	exit(exit_code);
//}
//
//int main(int argc, char** argv)
//{
//	if (argc != 2)
//		error_exit("[101]Usage: ./a.out [PORT_NUMBER]", 101);
//
//	server_port = atoi(argv[1]);
//	std::cout << "Port: " << server_port << std::endl;
//
//
//
//
//	// Create a Socket
//	int socket_fd = socket(PF_INET, SOCK_STREAM, 0);
//	if (socket_fd < 0)
//		error_exit("[102]socket() call for socket_fd failed", 102);
//
//
//
//	// Set sockaddr_in struct and bind()
//	sockaddr_in hint;
//	hint.sin_family = AF_INET;
//	hint.sin_port = htons(server_port);
//	hint.sin_addr.s_addr = INADDR_ANY;
//	int bind_result = bind(socket_fd, (sockaddr*)&hint, sizeof(hint));
//	if (bind_result < 0)
//	{
//		close(socket_fd);
//		error_exit("[103]Bind failed.", 103);
//	}
//
//
//
//	// listen()
//	int listen_result = listen(socket_fd, SOMAXCONN);
//	if (listen_result < 0)
//	{
//		close(socket_fd);
//		error_exit("[104]Listen failed.", 104);
//	}
//
//
//
//	fcntl(socket_fd, F_SETFL, O_NONBLOCK);
//	////////////////////////////////////////////////////////////////////////////////////
//	//     poll() magic                   poll() magic               poll() magic     //
//	////////////////////////////////////////////////////////////////////////////////////
//	nfds_t  nfds = 0;
//	struct pollfd *pollfds;
//	int numfds = 0;
//	int maxfds = 0;
//
//	// malloc() pollfds struct pointer
//	int max_clients = 5;
//	pollfds = (pollfd*)malloc(max_clients * sizeof(struct pollfd)); // max_clients is defined above,  you have to make the struct bigget later if necessary.
//	if (pollfds == NULL)
//	{
//		close(socket_fd);
//		error_exit("[105]pollfds malloc() failed.", 105);
//	}
//	maxfds = max_clients;
//
//	pollfds->fd = socket_fd; // We set the fd in the pollfds struct to our listening socket_fd.
//	pollfds->events = POLLIN; // we're interested if Input operations are possible, POLLIN for that.
//	numfds = 1;
//
//	// declare client addr structs
//	socklen_t                   client_addrlen;
//	struct  sockaddr_storage    client_socket_addr;
//	struct  sockaddr_in         *ptr_client_sockaddr;
//	struct  tnode               *root = NULL;
//
//
//
//	int new_fd; // used as return value when accepting a new connection.
//	char    buf[4096];
//	ssize_t bytes_received;
//	while (true)
//	{
//		nfds = numfds;
//
//
//		// poll() call to see what's available to read
//		if (poll(pollfds, nfds, 0) == -1) // the zero is to set it to non-blocking.
//		{
//			free(pollfds);
//			close(socket_fd);
//			error_exit("[106]poll() call failed.", 106);
//		}
//
//		// "Loop" through pollfds structs and see what's ready
//		for (int fd = 0; fd < (nfds+1); fd++)
//		{
//			if ((pollfds + fd)->fd <= 0) // fd shouldn't ever be 0(stdin)..
//				continue ;
//
//			if (((pollfds + fd)->revents & POLLIN) == POLLIN)   // whatever struct pollfds+fd is, it's ready for reading
//			{
//				// Check if the socket ready for reading is the listening socket (new connection to be accept()ed).
//				if ((pollfds + fd)->fd == socket_fd)
//				{
//					// New connection struct magic.
//					client_addrlen = sizeof(struct sockaddr_storage);
//					if ( (new_fd = accept(socket_fd, (struct sockaddr*)&client_socket_addr, &client_addrlen)) ==  -1)
//					{
//						free(pollfds);
//						close(socket_fd);
//						error_exit("[107]failed accept() call when new connection found.", 107);
//					}
//
//					// If numfds == maxfds then make more space in pollfds
//					if (maxfds == numfds)
//					{
//						pollfds = (pollfd*)realloc(pollfds, (maxfds + max_clients) * sizeof(struct pollfd));
//						if (pollfds == NULL)
//						{
//							close(socket_fd);
//							error_exit("[105]pollfds realloc() failed.", 105);
//						}
//						maxfds += max_clients;
//					}
//
//					numfds++; // One more client about to be connected.
//					(pollfds + numfds - 1)->fd = new_fd;
//					(pollfds + numfds - 1)->events = POLLIN;
//					(pollfds + numfds - 1)->revents = 0;
//					ptr_client_sockaddr = (sockaddr_in*)&client_socket_addr;
//					std::cout << "Accepted new connection from: " << inet_ntoa(ptr_client_sockaddr->sin_addr) << std::endl;
//
//
//				}
//				else    // Data from existing connection.
//				{
//					memset((void*)&buf, 0, 4096);
//					bytes_received = recv((pollfds + fd)->fd, (void*)buf, 4096, 0);
//					if (bytes_received < 0)
//					{
//						std::cout << "Error receiving data from established connection.\n";
//					}
//					else if (bytes_received == 0)
//					{
//						std::cout << "Connection closed by client.\n";
//						close((pollfds+fd)->fd);
//						(pollfds+fd)->fd = -1; // Set it to be ignored in the future.
//					}
//					else
//					{
//						std::cout << "Message received: " << std::string(buf, 0, bytes_received) << std::endl;
//						send((pollfds+fd)->fd, (void*)buf, bytes_received+1, 0);
//					}
//
//				}
//
//			} // End if statement checking if something is ready for reading
//		} // End for loop through pollfds
//	} // End infinate while loop.
//
//
//	close(socket_fd);
//	return (0);
//}



















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
	//separate_for_test((char *)argv[1]);
	smallTest("8888");
	//pollLoop("7777");

	return 0;
}


//#include <stdio.h>
//#include <stdlib.h>
//#include <sys/ioctl.h>
//#include <sys/poll.h>
//#include <sys/socket.h>
//#include <sys/time.h>
//#include <netinet/in.h>
//#include <errno.h>
//
//#define SERVER_PORT  12345
//
//#define TRUE             1
//#define FALSE            0
//
//int main (int argc, char *argv[])
//{
//  int    len, rc, on = 1;
//  int    listen_sd = -1, new_sd = -1;
//  int    desc_ready, end_server = FALSE, compress_array = FALSE;
//  int    close_conn;
//  char   buffer[80];
//  struct sockaddr_in6   addr;
//  int    timeout;
//  struct pollfd fds[200];
//  int    nfds = 1, current_size = 0, i, j;
//
//  /*************************************************************/
//  /* Create an AF_INET6 stream socket to receive incoming      */
//  /* connections on                                            */
//  /*************************************************************/
//  listen_sd = socket(AF_INET6, SOCK_STREAM, 0);
//  if (listen_sd < 0)
//  {
//	perror("socket() failed");
//	exit(-1);
//  }
//
//  /*************************************************************/
//  /* Allow socket descriptor to be reuseable                   */
//  /*************************************************************/
//  rc = setsockopt(listen_sd, SOL_SOCKET,  SO_REUSEADDR,
//				  (char *)&on, sizeof(on));
//  if (rc < 0)
//  {
//	perror("setsockopt() failed");
//	close(listen_sd);
//	exit(-1);
//  }
//
//  /*************************************************************/
//  /* Set socket to be nonblocking. All of the sockets for      */
//  /* the incoming connections will also be nonblocking since   */
//  /* they will inherit that state from the listening socket.   */
//  /*************************************************************/
//  rc = ioctl(listen_sd, FIONBIO, (char *)&on);
//  if (rc < 0)
//  {
//	perror("ioctl() failed");
//	close(listen_sd);
//	exit(-1);
//  }
//
//  /*************************************************************/
//  /* Bind the socket                                           */
//  /*************************************************************/
//  memset(&addr, 0, sizeof(addr));
//  addr.sin6_family      = AF_INET6;
//  memcpy(&addr.sin6_addr, &in6addr_any, sizeof(in6addr_any));
//  addr.sin6_port        = htons(SERVER_PORT);
//  rc = bind(listen_sd,
//			(struct sockaddr *)&addr, sizeof(addr));
//  if (rc < 0)
//  {
//	perror("bind() failed");
//	close(listen_sd);
//	exit(-1);
//  }
//
//  /*************************************************************/
//  /* Set the listen back log                                   */
//  /*************************************************************/
//  rc = listen(listen_sd, 32);
//  if (rc < 0)
//  {
//	perror("listen() failed");
//	close(listen_sd);
//	exit(-1);
//  }
//
//  /*************************************************************/
//  /* Initialize the pollfd structure                           */
//  /*************************************************************/
//  memset(fds, 0 , sizeof(fds));
//
//  /*************************************************************/
//  /* Set up the initial listening socket                        */
//  /*************************************************************/
//  fds[0].fd = listen_sd;
//  fds[0].events = POLLIN;
//  /*************************************************************/
//  /* Initialize the timeout to 3 minutes. If no                */
//  /* activity after 3 minutes this program will end.           */
//  /* timeout value is based on milliseconds.                   */
//  /*************************************************************/
//  timeout = (3 * 60 * 1000 * 1000);
//
//  /*************************************************************/
//  /* Loop waiting for incoming connects or for incoming data   */
//  /* on any of the connected sockets.                          */
//  /*************************************************************/
//  do
//  {
//	/***********************************************************/
//	/* Call poll() and wait 3 minutes for it to complete.      */
//	/***********************************************************/
//	printf("Waiting on poll()...\n");
//
//	rc = poll(fds, nfds, timeout);
//	  printf("poill done..\n");
//	/***********************************************************/
//	/* Check to see if the poll call failed.                   */
//	/***********************************************************/
//	if (rc < 0)
//	{
//	  perror("  poll() failed");
//	  break;
//	}
//
//	/***********************************************************/
//	/* Check to see if the 3 minute time out expired.          */
//	/***********************************************************/
//	if (rc == 0)
//	{
//	  printf("  poll() timed out.  End program.\n");
//	  break;
//	}
//
//
//	/***********************************************************/
//	/* One or more descriptors are readable.  Need to          */
//	/* determine which ones they are.                          */
//	/***********************************************************/
//	current_size = nfds;
//	for (i = 0; i < current_size; i++)
//	{
//	  /*********************************************************/
//	  /* Loop through to find the descriptors that returned    */
//	  /* POLLIN and determine whether it's the listening       */
//	  /* or the active connection.                             */
//	  /*********************************************************/
//	  if(fds[i].revents == 0)
//		continue;
//
//	  /*********************************************************/
//	  /* If revents is not POLLIN, it's an unexpected result,  */
//	  /* log and end the server.                               */
//	  /*********************************************************/
//	  if((fds[i].revents & POLLIN) != POLLIN)
//	  {
//		printf("  Error! revents = %d\n", fds[i].revents);
//		end_server = TRUE;
//		break;
//
//	  }
//	  if (fds[i].fd == listen_sd)
//	  {
//		/*******************************************************/
//		/* Listening descriptor is readable.                   */
//		/*******************************************************/
//		printf("  Listening socket is readable\n");
//
//		/*******************************************************/
//		/* Accept all incoming connections that are            */
//		/* queued up on the listening socket before we         */
//		/* loop back and call poll again.                      */
//		/*******************************************************/
//		do
//		{
//		  /*****************************************************/
//		  /* Accept each incoming connection. If               */
//		  /* accept fails with EWOULDBLOCK, then we            */
//		  /* have accepted all of them. Any other              */
//		  /* failure on accept will cause us to end the        */
//		  /* server.                                           */
//		  /*****************************************************/
//		  new_sd = accept(listen_sd, NULL, NULL);
//		  if (new_sd < 0)
//		  {
//			if (errno != EWOULDBLOCK)
//			{
//			  perror("  accept() failed");
//			  end_server = TRUE;
//			}
//			break;
//		  }
//
//		  /*****************************************************/
//		  /* Add the new incoming connection to the            */
//		  /* pollfd structure                                  */
//		  /*****************************************************/
//		  printf("  New incoming connection - %d\n", new_sd);
//		  fds[nfds].fd = new_sd;
//		  fds[nfds].events = POLLIN;
//		  nfds++;
//
//		  /*****************************************************/
//		  /* Loop back up and accept another incoming          */
//		  /* connection                                        */
//		  /*****************************************************/
//		} while (new_sd != -1);
//	  }
//
//	  /*********************************************************/
//	  /* This is not the listening socket, therefore an        */
//	  /* existing connection must be readable                  */
//	  /*********************************************************/
//
//	  else
//	  {
//		printf("  Descriptor %d is readable\n", fds[i].fd);
//		close_conn = FALSE;
//		/*******************************************************/
//		/* Receive all incoming data on this socket            */
//		/* before we loop back and call poll again.            */
//		/*******************************************************/
//
//		do
//		{
//		  /*****************************************************/
//		  /* Receive data on this connection until the         */
//		  /* recv fails with EWOULDBLOCK. If any other         */
//		  /* failure occurs, we will close the                 */
//		  /* connection.                                       */
//		  /*****************************************************/
//		  rc = recv(fds[i].fd, buffer, sizeof(buffer), 0);
//		  if (rc < 0)
//		  {
//			if (errno != EWOULDBLOCK)
//			{
//			  perror("  recv() failed");
//			  close_conn = TRUE;
//			}
//			break;
//		  }
//
//		  /*****************************************************/
//		  /* Check to see if the connection has been           */
//		  /* closed by the client                              */
//		  /*****************************************************/
//		  if (rc == 0)
//		  {
//			printf("  Connection closed\n");
//			close_conn = TRUE;
//			break;
//		  }
//
//		  /*****************************************************/
//		  /* Data was received                                 */
//		  /*****************************************************/
//		  len = rc;
//		  printf("  %d bytes received\n", len);
//
//		  /*****************************************************/
//		  /* Echo the data back to the client                  */
//		  /*****************************************************/
//		  rc = send(fds[i].fd, buffer, len, 0);
//		  if (rc < 0)
//		  {
//			perror("  send() failed");
//			close_conn = TRUE;
//			break;
//		  }
//
//		} while(TRUE);
//
//		/*******************************************************/
//		/* If the close_conn flag was turned on, we need       */
//		/* to clean up this active connection. This            */
//		/* clean up process includes removing the              */
//		/* descriptor.                                         */
//		/*******************************************************/
//		if (close_conn)
//		{
//		  close(fds[i].fd);
//		  fds[i].fd = -1;
//		  compress_array = TRUE;
//		}
//
//
//	  }  /* End of existing connection is readable             */
//	} /* End of loop through pollable descriptors              */
//
//	/***********************************************************/
//	/* If the compress_array flag was turned on, we need       */
//	/* to squeeze together the array and decrement the number  */
//	/* of file descriptors. We do not need to move back the    */
//	/* events and revents fields because the events will always*/
//	/* be POLLIN in this case, and revents is output.          */
//	/***********************************************************/
//	if (compress_array)
//	{
//	  compress_array = FALSE;
//	  for (i = 0; i < nfds; i++)
//	  {
//		if (fds[i].fd == -1)
//		{
//		  for(j = i; j < nfds; j++)
//		  {
//			fds[j].fd = fds[j+1].fd;
//		  }
//		  i--;
//		  nfds--;
//		}
//	  }
//	}
//
//  } while (end_server == FALSE); /* End of serving running.    */
//
//  /*************************************************************/
//  /* Clean up all of the sockets that are open                 */
//  /*************************************************************/
//  for (i = 0; i < nfds; i++)
//  {
//	if(fds[i].fd >= 0)
//	  close(fds[i].fd);
//  }
//}
