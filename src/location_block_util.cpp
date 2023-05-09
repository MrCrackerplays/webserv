#include <iostream>
#include "config.hpp"

enum	e_location_arguments {
	LAR_redirect,
	LAR_root,
	LAR_autoindex,
	LAR_set_cookie,
	LAR_require_cookie,
	LAR_index,
	LAR_client_body_limit,
	LAR_methods,
	LAR_cgi,
	LAR_error_page,
	LAR_save_location,
	LAR_invalid
};

static e_location_arguments	str_to_e_loc(std::string & str) {
	//string.rfind(substring, 0) == 0 checks if string starts with substring
	if (str.rfind("redirect", 0) == 0)
		return LAR_redirect;
	if (str.rfind("root", 0) == 0)
		return LAR_root;
	if (str.rfind("autoindex", 0) == 0)
		return LAR_autoindex;
	if (str.rfind("set_cookie", 0) == 0)
		return LAR_set_cookie;
	if (str.rfind("require_cookie", 0) == 0)
		return LAR_require_cookie;
	if (str.rfind("index", 0) == 0)
		return LAR_index;
	if (str.rfind("max_body", 0) == 0)
		return LAR_client_body_limit;
	if (str.rfind("methods", 0) == 0)
		return LAR_methods;
	if (str.rfind("cgi", 0) == 0)
		return LAR_cgi;
	if (str.rfind("error_page", 0) == 0)
		return LAR_error_page;
	if (str.rfind("save_location", 0) == 0)
		return LAR_save_location;
	return LAR_invalid;
}

static void	handle_redirect(size_t & i, std::string & line, Location & loc) {
	skip_space(line, i);
	size_t	code_start = i;
	while (std::isdigit(line[i]))
		i++;
	size_t	code_end = i;
	if (code_start == code_end) {
		std::cerr << "'" << line << "'" << std::endl;
		std::cerr << "too few arguments for redirect" << std::endl;
		throw ConfigFormatException();
	}
	skip_space(line, i);
	size_t	url_start = i;
	while (i < line.length() && !std::isspace(line[i]))
		i++;
	size_t	url_end = i;
	if (url_start == url_end) {
		std::cerr << "'" << line << "'" << std::endl;
		std::cerr << "too few arguments for redirect" << std::endl;
		throw ConfigFormatException();
	}
	skip_space(line, i);
	if (i != line.length()) {
		std::cerr << "'" << line << "'" << std::endl;
		std::cerr << "too many arguments for redirect" << std::endl;
		throw ConfigFormatException();
	}
	loc.setRedirect(std::pair<std::string, std::string>(
		line.substr(code_start, code_end - code_start), line.substr(url_start, url_end - url_start)));
}

static void	handle_root(size_t & i, std::string & line, Location & loc) {
	skip_space(line, i);
	size_t	root_start = i;
	while (i < line.length() && !std::isspace(line[i]))
		i++;
	size_t	root_end = i;
	if (root_start == root_end) {
		std::cerr << "'" << line << "'" << std::endl;
		std::cerr << "no value for root" << std::endl;
		throw ConfigFormatException();
	}
	skip_space(line, i);
	if (i != line.length()) {
		std::cerr << "'" << line << "'" << std::endl;
		std::cerr << "too many arguments for root" << std::endl;
		throw ConfigFormatException();
	}
	if (line[root_end - 1] != '/') {
		std::cerr << "'" << line << "'" << std::endl;
		std::cerr << "root must end with a '/'" << std::endl;
		throw ConfigFormatException();
	}
	loc.setRoot(line.substr(root_start, root_end - root_start));
}

static void	handle_autoindex(size_t & i, std::string & line, Location & loc) {
	skip_space(line, i);
	size_t	autoindex_start = i;
	while (i < line.length() && !std::isspace(line[i]))
		i++;
	size_t	autoindex_end = i;
	if (autoindex_start == autoindex_end) {
		std::cerr << "'" << line << "'" << std::endl;
		std::cerr << "no value for autoindex" << std::endl;
		throw ConfigFormatException();
	}
	skip_space(line, i);
	if (i != line.length()) {
		std::cerr << "'" << line << "'" << std::endl;
		std::cerr << "too many arguments for autoindex" << std::endl;
		throw ConfigFormatException();
	}
	loc.setDirectoryListing(line.substr(autoindex_start, autoindex_end - autoindex_start) == "on");
}

static void	handle_set_cookie(size_t & i, std::string & line, Location & loc) {
	skip_space(line, i);
	if (i == line.length()) {
		std::cerr << "'" << line << "'" << std::endl;
		std::cerr << "no value for set_cookie" << std::endl;
		throw ConfigFormatException();
	}
	loc.setSetCookie(line.substr(i));
}

static void	handle_require_cookie(size_t & i, std::string & line, Location & loc) {
	skip_space(line, i);
	if (i == line.length()) {
		std::cerr << "'" << line << "'" << std::endl;
		std::cerr << "no value for require_cookie" << std::endl;
		throw ConfigFormatException();
	}
	loc.setRequireCookie(line.substr(i));
}

static void	handle_index(size_t & i, std::string & line, Location & loc) {
	skip_space(line, i);
	size_t	index_start = i;
	while (i < line.length() && !std::isspace(line[i]))
		i++;
	size_t	index_end = i;
	if (index_start == index_end) {
		std::cerr << "'" << line << "'" << std::endl;
		std::cerr << "no value for index" << std::endl;
		throw ConfigFormatException();
	}
	skip_space(line, i);
	if (i != line.length()) {
		std::cerr << "'" << line << "'" << std::endl;
		std::cerr << "too many arguments for index" << std::endl;
		throw ConfigFormatException();
	}
	loc.setDefaultFile(line.substr(index_start, index_end - index_start));
}

static void	handle_client_body_limit(size_t & i, std::string & line, Location & loc) {
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
	int	size = std::atoi(line.substr(cbl_start, cbl_end - cbl_start).c_str());
	if (size < 0 || size > MAX_BODY_LIMIT) {
		std::cerr << "'" << line << "'" << std::endl;
		std::cerr << "max_body not within valid range" << std::endl;
		throw ConfigFormatException();
	}
	loc.setClientBodyLimit(size);
}

static void	handle_methods(size_t & i, std::string & line, Location & loc) {
	skip_space(line, i);
	if (i == line.length()) {
		std::cerr << "'" << line << "'" << std::endl;
		std::cerr << "no value for methods" << std::endl;
		throw ConfigFormatException();
	}
	size_t	method_start;
	while (i < line.length()) {
		method_start = i;
		while (i < line.length() && !std::isspace(line[i]))
			i++;
		loc.addHTTPMethod(line.substr(method_start, i - method_start));
		skip_space(line, i);
	}
}

static void	handle_cgi(size_t & i, std::string & line, Location & loc) {
	skip_space(line, i);
	if (i == line.length()) {
		std::cerr << "'" << line << "'" << std::endl;
		std::cerr << "no value for cgi" << std::endl;
		throw ConfigFormatException();
	}
	size_t	cgi_start;
	while (i < line.length()) {
		cgi_start = i;
		while (i < line.length() && !std::isspace(line[i]))
			i++;
		loc.addCGIExtension(line.substr(cgi_start, i - cgi_start));
		skip_space(line, i);
	}
}

static void	handle_error_page(size_t & i, std::string & line, Location & loc) {
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
	loc.addError(line.substr(code_start, code_end - code_start), line.substr(page_start, page_end - page_start));
}

static void	handle_save_location(size_t & i, std::string & line, Location & loc) {
	skip_space(line, i);
	size_t	save_location_start = i;
	while (i < line.length() && !std::isspace(line[i]))
		i++;
	size_t	save_location_end = i;
	if (save_location_start == save_location_end) {
		std::cerr << "'" << line << "'" << std::endl;
		std::cerr << "no value for save_location" << std::endl;
		throw ConfigFormatException();
	}
	skip_space(line, i);
	if (i != line.length()) {
		std::cerr << "'" << line << "'" << std::endl;
		std::cerr << "too many arguments for save_location" << std::endl;
		throw ConfigFormatException();
	}
	if (line[save_location_end - 1] != '/') {
		std::cerr << "'" << line << "'" << std::endl;
		std::cerr << "save_location must end with a '/'" << std::endl;
		throw ConfigFormatException();
	}
	loc.setSaveLocation(line.substr(save_location_start, save_location_end - save_location_start));
}

void	location_line(std::string & line, bool & in_location, Location & loc) {
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
		if (i - 1 == key_start && line[key_start] == '}') {
			in_location = false;
			return;
		}
		std::cerr << "'" << line << "'" << std::endl;
		std::cerr << "location key without value" << std::endl;
		throw ConfigFormatException();//key without value
	}
	std::string	key = line.substr(key_start, i - key_start);
	switch (str_to_e_loc(key))
	{
	case LAR_redirect:
		handle_redirect(i, line, loc);
		break;
	case LAR_root:
		handle_root(i, line, loc);
		break;
	case LAR_autoindex:
		handle_autoindex(i, line, loc);
		break;
	case LAR_set_cookie:
		handle_set_cookie(i, line, loc);
		break;
	case LAR_require_cookie:
		handle_require_cookie(i, line, loc);
		break;
	case LAR_index:
		handle_index(i, line, loc);
		break;
	case LAR_client_body_limit:
		handle_client_body_limit(i, line, loc);
		break;
	case LAR_methods:
		handle_methods(i, line, loc);
		break;
	case LAR_cgi:
		handle_cgi(i, line, loc);
		break;
	case LAR_error_page:
		handle_error_page(i, line, loc);
		break;
	case LAR_save_location:
		handle_save_location(i, line, loc);
		break;

	case LAR_invalid:
	default://default should be impossible to happen but it's there for clarity
		std::cerr << "'" << line << "'" << std::endl;
		std::cerr << "invalid key '" << key << "'" << std::endl;
		throw ConfigFormatException();
		break;
	}
}