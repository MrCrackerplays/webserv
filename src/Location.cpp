#include "Location.hpp"

Location::Location(std::string path,
	std::pair<std::string, std::string> redirect, std::string root, bool directory_listing,
	std::string default_file, std::string set_cookie, std::string require_cookie, size_t client_body_limit)
	: _path(path), _root(root), _directory_listing(directory_listing), _set_cookie(set_cookie), _require_cookie(require_cookie),
	_default_file(default_file), _client_body_limit(client_body_limit), _redirect(redirect), _methods(), _cgis(), _error_pages() {
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
	this->_require_cookie = rhs._require_cookie;
	this->_set_cookie = rhs._set_cookie;
	return (*this);
}

void	Location::addCGIExtension(std::string ext) {
	this->_cgis.push_back(ext);
}

void	Location::addHTTPMethod(std::string method) {
	this->_methods.push_back(method);
}

void	Location::addError(std::string error_code, std::string path) {
	this->_error_pages.insert(std::pair<std::string, std::string>(error_code, path));
}

std::ostream& operator<<(std::ostream& out, const Location& loc) {
	out << "{" << loc.getPath();
	if (loc.getRedirect().first != "")
		out << ", redirect=" << loc.getRedirect().first << " " << loc.getRedirect().second;
	if (loc.getRoot() != "")
		out << ", root=" << loc.getRoot();
	out << ", directory_listing=" << (loc.getDirectoryListing() ? "true" : "false");
	if (loc.getDefaultFile() != "")
		out << ", default_file=" << loc.getDefaultFile();
	if (loc.getSetCookie() != "")
		out << ", set_cookie=" << loc.getSetCookie();
	if (loc.getRequireCookie() != "")
		out << ", require_cookie=" << loc.getRequireCookie();
	if (loc.getClientBodyLimit() != 0)
		out << ", client_body_limit=" << loc.getClientBodyLimit();
	if (loc.getMethods().size() > 0) {
		std::vector<std::string> methods = loc.getMethods();
		out << ", methods={";
		for (std::vector<std::string>::const_iterator it = methods.begin(); it != methods.end(); it++) {
			if (it != methods.begin())
				out << ", ";
			out << *it;
		}
		out << "}";
	}
	if (loc.getCGIs().size() > 0) {
		std::vector<std::string> cgis = loc.getCGIs();
		out << ", cgis={";
		for (std::vector<std::string>::const_iterator it = cgis.begin(); it != cgis.end(); it++) {
			if (it != cgis.begin())
				out << ", ";
			out << *it;
		}
		out << "}";
	}
	if (loc.getErrorPages().size() > 0) {
		std::map<std::string, std::string> error_pages = loc.getErrorPages();
		out << ", error_pages={";
		for (std::map<std::string, std::string>::const_iterator it = error_pages.begin(); it != error_pages.end(); it++) {
			if (it != error_pages.begin())
				out << ", ";
			out << it->first << "=" << it->second;
		}
		out << "}";
	}
	out << "}";
	return (out);
}
