#ifndef LOCATION_HPP
# define LOCATION_HPP
# include <string>
# include <vector>
# include <map>
# include <ostream>

class Location {
protected:
	std::string		_path;
	std::string		_root;
	bool			_directory_listing;
	std::string		_set_cookie;
	std::string		_require_cookie;
	std::string		_default_file;
	ssize_t			_client_body_limit;
	std::string		_save_location;
	std::pair<std::string, std::string>	_redirect;
	std::vector<std::string>			_methods;
	std::vector<std::string>			_cgis;
	std::map<std::string, std::string>	_error_pages;
public:
	Location(std::string path,
			std::string root,
			std::pair<std::string, std::string> redirect = std::pair<std::string, std::string>("", ""),
			bool directory_listing = false,
			std::string default_file = "index.html",
			std::string set_cookie = "",
			std::string require_cookie = "",
			ssize_t client_body_limit = -1,
			std::string save_location = "");
	Location(Location const & src);

	~Location(void);

	Location & operator=(Location const & rhs);

	//getters and setters
	std::string	getPath(void) const { return (this->_path); };
	std::string	getRoot(void) const { return (this->_root); };
	bool		getDirectoryListing(void) const { return (this->_directory_listing); };
	std::string	getDefaultFile(void) const { return (this->_default_file); };
	ssize_t		getClientBodyLimit(void) const { return (this->_client_body_limit); };
	std::string	getSetCookie(void) const { return (this->_set_cookie); };
	std::string	getRequireCookie(void) const { return (this->_require_cookie); };
	std::pair<std::string, std::string>	getRedirect(void) const { return (this->_redirect); };
	std::string	getSaveLocation(void) const { return (this->_save_location); };

	void	setPath(std::string path) { this->_path = path; };
	void	setRoot(std::string root) { this->_root = root; };
	void	setDirectoryListing(bool directory_listing) { this->_directory_listing = directory_listing; };
	void	setDefaultFile(std::string default_file) { this->_default_file = default_file; };
	void	setClientBodyLimit(size_t client_body_limit) { this->_client_body_limit = client_body_limit; };
	void	setSetCookie(std::string set_cookie) { this->_set_cookie = set_cookie; };
	void	setRequireCookie(std::string require_cookie) { this->_require_cookie = require_cookie; };
	void	setRedirect(std::pair<std::string, std::string> redirect) { this->_redirect = redirect; };
	void	setSaveLocation(std::string save_location) { this->_save_location = save_location; };



	//getters and setters for containers
	std::vector<std::string>			getMethods(void) const { return (this->_methods); };
	std::vector<std::string>			getCGIs(void) const { return (this->_cgis); };
	std::map<std::string, std::string>	getErrorPages(void) const { return (this->_error_pages); };

	void	addCGIExtension(std::string);
	void	addHTTPMethod(std::string);
	void	addError(std::string error_code, std::string path);
private:
	Location(void);
};

std::ostream& operator<<(std::ostream& out, const Location& loc);

#endif
