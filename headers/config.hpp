#ifndef CONFIG_HPP
# define CONFIG_HPP
# include "Server.hpp"

void	initialize_servers(std::map<std::string, std::vector<Server> > & servers, std::string & config_file);

class ConfigFormatException : public std::exception {
	public:
		const char* what() const throw();
};

class ConfigLoadException : public std::exception {
	public:
		const char* what() const throw();
};

class ConfigPortFormatException : public std::exception {
	public:
		const char* what() const throw();
};

inline void	skip_space(std::string & str, size_t & i) {
	while (i < str.length() && std::isspace(str[i]))
		i++;
}

void server_line(std::string & line, bool & in_location, Server & serv, std::string & location);
void location_line(std::string & line, bool & in_location, Location & loc);

#endif
