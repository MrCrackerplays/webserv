#include <iostream>
#include "config.hpp"

enum	e_server_arguments {
	SAR_host,
	SAR_port,
	SAR_server_names,
	SAR_client_body_limit,
	SAR_error_page,
	SAR_location,
	SAR_invalid
};

static e_server_arguments	str_to_e_serv(std::string & str) {
	//string.rfind(substring, 0) == 0 checks if string starts with substring
	if (str.rfind("host", 0) == 0)
		return SAR_host;
	if (str.rfind("port", 0) == 0)
		return SAR_port;
	if (str.rfind("server_names", 0) == 0)
		return SAR_server_names;
	if (str.rfind("max_body", 0) == 0)
		return SAR_client_body_limit;
	if (str.rfind("error_page", 0) == 0)
		return SAR_error_page;
	if (str.rfind("location", 0) == 0)
		return SAR_location;
	return SAR_invalid;
}

static void	handle_server_names(size_t & i, std::string & line, Server & serv) {
	while (i < line.length()) {
		if (std::isspace(line[i])) {
			skip_space(line, i);
		} else {
			size_t	name_start = i;
			while (i < line.length() && !std::isspace(line[i]))
				i++;
			serv.addName(line.substr(name_start, i - name_start));
		}
	}
}

static void	handle_client_body_limit(size_t & i, std::string & line, Server & serv) {
	skip_space(line, i);
	size_t	cbl_start = i;
	while (std::isdigit(line[i]))
		i++;
	size_t	cbl_end = i;
	if (cbl_start == cbl_end) {
		std::cerr << "'" << line << "'" << std::endl;
		std::cerr << "no value for max_body" << std::endl;
		throw ConfigFormatException();
	}
	skip_space(line, i);
	if (i != line.length()) {
		std::cerr << "'" << line << "'" << std::endl;
		std::cerr << "too many arguments for max_body" << std::endl;
		throw ConfigFormatException();
	}
	serv.setClientBodyLimit(std::atoi(line.substr(cbl_start, cbl_end - cbl_start).c_str()));
}

static void	handle_error_page(size_t & i, std::string & line, Server & serv) {
	skip_space(line, i);
	size_t	code_start = i;
	while (std::isdigit(line[i]))
		i++;
	size_t	code_end = i;
	if (code_start == code_end) {
		std::cerr << "'" << line << "'" << std::endl;
		std::cerr << "too few arguments for error_page" << std::endl;
		throw ConfigFormatException();//no arguments
	}
	skip_space(line, i);
	size_t	page_start = i;
	while (i < line.length() && !std::isspace(line[i]))
		i++;
	size_t	page_end = i;
	if (page_start == page_end) {
		std::cerr << "'" << line << "'" << std::endl;
		std::cerr << "too few arguments for error_page" << std::endl;
		throw ConfigFormatException();//only 1 argument
	}
	skip_space(line, i);
	if (i != line.length()) {
		std::cerr << "'" << line << "'" << std::endl;
		std::cerr << "too many arguments for error_page" << std::endl;
		throw ConfigFormatException();//more than 2 arguments
	}
	serv.addError(line.substr(code_start, code_end - code_start), line.substr(page_start, page_end - page_start));
}

static std::string	handle_location(size_t & i, std::string & line, Server & serv) {
	std::string	path = "";
	skip_space(line, i);
	if (line[i] != '{') {
		size_t	path_start = i;
		while (i < line.length() && line[i] != '{')
			i++;
		if (line[i] == '{') {
			if (i != line.length() - 1) {
				size_t tmp = i + 1;
				skip_space(line, tmp);
				if (tmp != line.length()) {
					std::cerr << "'" << line << "'" << std::endl;
					std::cerr << "non-whitespace after '{' in location" << std::endl;
					throw ConfigFormatException();
				}
			}
			i--;
			while (std::isspace(line[i]))
				i--;
			i++;
			path = line.substr(path_start, i - path_start);
		} else {
			std::cerr << "'" << line << "'" << std::endl;
			std::cerr << "did not find '{' after location" << std::endl;
			throw ConfigFormatException();
		}
	}
	Location	l(path);
	serv.addLocation(path, l);
	return (path);
}

void	server_line(std::string & line, bool & in_location, Server & serv, std::string & location) {
	size_t i = 0;
	skip_space(line, i);
	if (i == line.length())
		return;//empty line
	size_t key_start = i;
	while (i < line.length() && !std::isspace(line[i]))
		i++;
	size_t	j = i;
	skip_space(line, j);
	if (i == line.length() || j == line.length()) {
		std::cerr << "'" << line << "'" << std::endl;
		std::cerr << "server key without value" << std::endl;
		throw ConfigFormatException();//key without value
	}
	std::string	key = line.substr(key_start, i - key_start);

	switch (str_to_e_serv(key))
	{
	case SAR_server_names:
		handle_server_names(i, line, serv);
		break;

	case SAR_client_body_limit:
		handle_client_body_limit(i, line, serv);
		break;

	case SAR_error_page:
		handle_error_page(i, line, serv);
		break;

	case SAR_location:
		in_location = true;
		location = handle_location(i, line, serv);
		break;

	case SAR_invalid:
		std::cerr << "'" << line << "'" << std::endl;
		std::cerr << "invalid key '" << key << "'" << std::endl;
		throw ConfigFormatException();
		break;

	default:
		//already handled keys (port, host)
		break;
	}
}