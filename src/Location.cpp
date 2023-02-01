#include <location.hpp>

Location::Location(std::string path, std::string redirect, std::string root, bool directory_listing,
	std::string default_file, size_t client_body_limit, std::vector<std::string> methods,
	std::vector<std::string> cgis, std::map<std::string, std::string> error_pages)
	: _path(path), _redirect(redirect), _directory_listing(directory_listing), _default_file(default_file),
	_client_body_limit(client_body_limit), _methods(methods), _cgis(cgis), _error_pages(error_pages) {
}

Location::Location(Location const & src) {
	*this = src;
}

Location::~Location(void) {
	;
}

Location & Location::operator=(Location const & rhs) {
	if (this == &rhs)
		return (*this);
	this->_path = rhs._path;
	this->_redirect = rhs._redirect;
	this->_root = rhs._root;
	this->_directory_listing = rhs._directory_listing;
	this->_default_file = rhs._default_file;
	this->_client_body_limit = rhs._client_body_limit;
	this->_methods = rhs._methods;
	this->_cgis = rhs._cgis;
	this->_error_pages = rhs._error_pages;
	return (*this);
}
