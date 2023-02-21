#include <iostream>
#include "Server.hpp"
#include "config.hpp"

// void	funk(Server &server_block) {
// 	Location some_location_block("/shii");//, "yesredirect", "root", false, "index.html", "thissetcookie", "thatrequirecookie", 50);
// 	server_block.addLocation(some_location_block.getPath(), some_location_block);
// }

int	main(int argc, char **argv) {
	std::string	config_file = "configs/default.conf";
	if (argc > 2) {
		std::cout << "invalid number of arguments" << std::endl;
		return (1);
	}
	if (argc == 2)
		config_file = argv[1];
	std::map<std::string, std::vector<Server> > servers;
	try {
		initialize_servers(servers, config_file);
		std::map<std::string, std::vector<Server> >::iterator it;
		for (it = servers.begin(); it != servers.end(); it++)
		{
			std::cout << it->first << std::endl;
			for(size_t i=0; i < it->second.size(); i++){
				std::cout << "FOUNDSERVER: " << it->second[i] << std::endl;
			}
		}
	} catch(const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return (1);
	}
	
	// Server	server_block(8080, "localhost", 9, "kane");
	// server_block.addError("404", "yuh");
	// funk(server_block);
	// std::cout << server_block << std::endl;
	// std::cout << s.getErrorPage("500", "/shii/yuh/yuk") << std::endl;
}
