#include <iostream>
#include "Server.hpp"
#include "config.hpp"

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
}
