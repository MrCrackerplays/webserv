#ifndef LOCATION_HPP
# define LOCATION_HPP
# include <string>
# include <vector>
# include <map>

class Location {
public:
	std::string		_path;
	std::string		_redirect;
	std::string		_root;
	bool			_directory_listing;
	std::string		_default_file;
	size_t			_client_body_limit;
	std::vector<std::string>			_methods;
	std::vector<std::string>			_cgis;
	std::map<std::string, std::string>	_error_pages;
public:
	Location(std::string path,
			std::string redirect = "",
			std::string root = "",
			bool directory_listing = false,
			std::string default_file = "",
			size_t client_body_limit = 0,
			std::vector<std::string> methods = {},
			std::vector<std::string> cgis = {},
			std::map<std::string, std::string> error_pages = {});
	Location(Location const & src);

	~Location(void);

	Location & operator=(Location const & rhs);

	// void	setRedirect(std::string);
	// void	setRoot(std::string);
	// void	setDirerctoryListing(bool);
	// void	setDefaultFile(std::string);
	// void	addCGIExtension(std::string);
	// void	setErrorPage(std::string error_code, std::string file);
private:
	Location(void);
	
};

#endif
