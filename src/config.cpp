#include <iostream>
#include <fstream>
#include <sstream>
#include "config.hpp"

const char* ConfigFormatException::what() const throw() {
	return ("Error loading config: Config is not formatted correctly!");
}

const char* ConfigPortFormatException::what() const throw() {
	return ("Error loading config: A port is not formatted correctly!");
}

const char* ConfigLoadException::what() const throw() {
	return ("Error loading config: Could not read from file! (file might be empty)");
}

static std::string	extract_server_block(std::string & config, size_t & i) {
	const std::string	server_block_start = "server";
	int		depth = 0;
	size_t	start;

	skip_space(config, i);
	if (config.compare(i, server_block_start.size(), server_block_start) != 0)
		throw ConfigFormatException();
	i += server_block_start.size();
	skip_space(config, i);
	if (config[i] != '{')
		throw ConfigFormatException();
	depth++;
	i++;
	start = i;

	while (depth != 0) {
		//inside a config block
		skip_space(config, i);
		while (i < config.size() && config[i] != '{' && config[i] != '}')
			i++;
		if (config[i] == '{') {
			depth++;
			i++;
		} else if (config[i] == '}') {
			//found end of block
			depth--;
			i++;
		} else
			throw ConfigFormatException();

		//currently only accept a location block inside a server block but the above implementation
		//allows for adding more blocks within blocks later on without having to update a lot of code
		if (depth > 2)
			throw ConfigFormatException();
	};

	return (config.substr(start, i - 1 - start));
}

static std::string	read_file(std::string filename) {
	std::ifstream conf(filename);
	std::stringstream buffer;
	if (!(buffer << conf.rdbuf()))
		throw ConfigLoadException();
	return (buffer.str());
}

static std::string	get_server_value(std::istringstream & serv, std::string key) {
	std::string	line;
	size_t	i;
	while (std::getline(serv, line)) {
		i = 0;
		skip_space(line, i);
		if (line.find(key) == i) {
			i += key.length();
			skip_space(line, i);
			size_t j = i;
			while (j < line.length() && !std::isspace(line[j]))
				j++;
			size_t k = j;
			skip_space(line, k);
			if (k != line.length()) {
				std::cerr << "multiple values for '" << key << "'" << std::endl;
				throw ConfigFormatException();
			}
			serv.clear();
			serv.seekg(0);
			return line.substr(i, j - i);
		}
	}
	std::cerr << "missing value for '" << key << "'" << std::endl;
	throw ConfigFormatException();
}

static void	add_server(std::map<std::string, std::vector<Server> > & servers, std::string & block) {
	int	port = 0;
	std::istringstream	serv(block);
	std::string	host = get_server_value(serv, "host");
	std::string	port_str = get_server_value(serv, "port");
	if (port_str.length() > 5) {
		std::cerr << "port too long " << port_str.length() << " '" << port_str << "'" << std::endl;
		throw ConfigPortFormatException();
	}
	if (port_str.find_first_not_of("0123456789") != std::string::npos) {
		std::cerr << "found a non-numerical character" << std::endl;
		throw ConfigPortFormatException();
	}
	port = std::atoi(port_str.c_str());
	if (port < 1 || port > 65535) {
		std::cerr << "port out of range " << port << " from '" << port_str << "'" << std::endl;
		throw ConfigPortFormatException();
	}
	std::vector<Server> & vec = servers[host + port_str];
	vec.push_back(Server(port, host));
	Server & server = vec.back();
	std::string	line;
	std::string	location;
	Location *	loc = NULL;
	bool	in_location = false;
	while (std::getline(serv, line)) {
		if (!in_location) {
			server_line(line, in_location, server, location);
		} else {
			if (!loc) {
				loc = server.getLocation(location);
				if (!loc) {
					std::cerr << "'" << line << "'" << std::endl;
					std::cerr << "location '" << location << "' not found despite adding, this should be impossible" << std::endl;
					throw std::exception();
				}
			}
			location_line(line, in_location, *loc);
			if (!in_location)
				loc = NULL;
		}
	}
}

void	initialize_servers(std::map<std::string, std::vector<Server> > & servers, std::string & config_file) {
	size_t		i = 0;
	std::string	block;

	std::string	config = read_file(config_file);
	while (i < config.size()) {
		block = extract_server_block(config, i);
		add_server(servers, block);
	}
	std::cout << "past while" << std::endl;
	if (servers.size() == 0)
		throw std::exception();
}